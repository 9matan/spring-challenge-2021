from pathlib import Path
import argparse
import sys
import os

class FileInfo:
    def __init__(self, fileName):
        self.includedFileNames = []
        self.lines = []
        self.fileName = fileName # absolute path
        self.cppFileName = ""

class Packer:
    def __init__(self):
        self.handledFiles = {}
        self.pendingFiles = {}
        self.collectedFiles = set()
        self.additionalIncludes = []

    def add_additional_include_path(self, pathStr):
        path = Path(pathStr)
        if path.exists() and path.is_dir():
            self.additionalIncludes.append(path)

    def find_including_file_name(self, fileName, includingFileName):
        searchingDirs = [str(Path(fileName).parent)]
        searchingDirs.extend(self.additionalIncludes)

        for dir in searchingDirs:
            path = Path(dir).joinpath(includingFileName).resolve().absolute()
            if path.exists() and path.is_file():
                return str(path)
        return ""

    def add_pending_file(self, fileName):
        if fileName == "":
            return
        if fileName in self.handledFiles or fileName in self.pendingFiles:
            return
        filePath = Path(fileName)
        if filePath.exists() and filePath.is_file():
            self.pendingFiles[fileName] = FileInfo(fileName)

    def process_file(self, fileNameToPrecess):
        self.fileNameToProcess = fileNameToPrecess
        self.handledFiles = {}
        self.pendingFiles = {}
        self.collectedFiles = set()
        self.add_pending_file(fileNameToPrecess)
        self.handle_all_pending_files()

    def write_result_to_file(self, outFileName, additionalInfo=False):
        with Path(outFileName).open('w') as file:
            file.writelines(self.collect_all_files_lines())
            if additionalInfo:
                file.write("\n/* Handled files:")
                for fileName in self.handledFiles.keys():
                    file.write("\n   " + fileName)
                file.write("\n*/")

    def handle_all_pending_files(self):
        while self.pendingFiles:
            key, val = self.pendingFiles.popitem()
            self.handle_file(val)

    def handle_file(self, fileInfo):
        filePath = Path(fileInfo.fileName)
        if not filePath.exists() or not filePath.is_file():
            return
        with Path(fileInfo.fileName).open() as file:
            fileInfo.lines = file.readlines()        
        if(fileInfo.fileName.endswith(".h")):
            fileInfo.cppFileName = fileInfo.fileName[:-2] + ".cpp"
            self.add_pending_file(fileInfo.cppFileName)
        
        isNewLineInTheEndOfPrev = False
        isMultiLineCommentStarted = False
        for index, line in enumerate(fileInfo.lines):
            if "[Ignore]" in line:
                line = ""
            else :
                if "[Enable]" in line and line.startswith("//"):
                    line = line[2:]
                    line.replace("[Enable]", "")
                # remove blanks
                line = line.strip()
                line = line.replace("\t", "")
                line = line.replace("\n", "")
                if line.startswith("/*") and not line.endswith("*/"):
                    isMultiLineCommentStarted = True
                    line = ""
                elif line.endswith("*/") and not line.startswith("/*"):
                    isMultiLineCommentStarted = False
                    line = ""
                elif isMultiLineCommentStarted:
                    line = ""
                elif line.startswith("#pragma once"):
                    line = ""
                else:
                    # remove comments
                    commentPos = line.find("//")
                    if commentPos > -1:
                        line = line[:commentPos]
                    # handling of preproc
                    isPreproc = line.startswith('#')
                    shouldStartWithNewLine = isPreproc
                    if shouldStartWithNewLine and not isNewLineInTheEndOfPrev:
                        line = '\n' + line
                    addNewLineCharAfter = line.endswith('\\') or isPreproc
                    isNewLineInTheEndOfPrev = False
                    if addNewLineCharAfter:
                        line += "\n"
                        isNewLineInTheEndOfPrev = True
                # handle includes
                if self.handle_include_file(fileInfo, line):
                    line = ""
            fileInfo.lines[index] = line
        # remove empty lines
        fileInfo.lines = [line for line in fileInfo.lines if line]
        self.handledFiles[fileInfo.fileName] = fileInfo

    def get_include_file_name(self, line):
        if "#include" not in line:
            return (False, "", False)
        isSystem = False
        start = line.find('\"')
        if start < 0:
            start = line.find('<')
            if start < 0:
                return (False, "", False)
            else:
                isSystem = True
        end = -1
        if isSystem:
            end = line.find('>', start + 1)
        else:
            end = line.find('\"', start + 1)
        fileName = line[start+1:end]
        return (True, fileName, isSystem)

    def handle_include_file(self, fileInfo, line):
        isFound, fileName, isSystem = self.get_include_file_name(line)
        if isFound and not isSystem:
            fileName = self.find_including_file_name(fileInfo.fileName, fileName)
            if fileName != "":
                fileInfo.includedFileNames.append(fileName)
                self.add_pending_file(fileName)
            return True
        return False

    def collect_all_files_lines(self):
        self.collectedFiles = set()
        filesLines = self.collect_files_lines(self.fileNameToProcess)
        cppFilesLines = []
        for fileInfo in self.handledFiles.values():
            cppFileName = fileInfo.cppFileName
            cppFilesLines.extend(self.collect_files_lines(cppFileName))
        filesLines.extend(cppFilesLines)
        self.remove_dublicated_includes(filesLines)
        return filesLines

    def collect_files_lines(self, fileName):
        if fileName in self.collectedFiles:
            return []
        if fileName not in self.handledFiles:
            return []
        self.collectedFiles.add(fileName)
        lines = []
        fileInfo = self.handledFiles[fileName]
        for includeFileName in fileInfo.includedFileNames:
            lines.extend(self.collect_files_lines(includeFileName))
        lines.extend(fileInfo.lines)
        if len(lines) > 0:
            lines[-1] += "\n"
        return lines

    def remove_dublicated_includes(self, lines):
        systemIncludes = set()
        for index, line in enumerate(lines):
            isFound, fileName, isSystem = self.get_include_file_name(line)
            if isFound and isSystem:
                if fileName in systemIncludes:
                    line = ""
                else:
                    systemIncludes.add(fileName)
            lines[index] = line

def main():
    ap = argparse.ArgumentParser()    
    ap.add_argument('-src', '--source-file', help="Source cpp file to work with", default='', required=True)
    ap.add_argument('-dst', '--destination-file', help="File to write the result", default='', required=True)
    ap.add_argument('-addp', '--additional-path', help="The list of paths to folders to search for files from includes", nargs='*', required=False)
    # save all files' names relatively to the root
    # search for the including file relatively to the current file and in additional folders
    args = vars(ap.parse_args())

    fileNameToProcess = args['source_file']
    outFileName = args['destination_file']

    # set working dir
    filePath = Path(fileNameToProcess).absolute()
    if not filePath.is_file():
        print("ERROR: invalid file path")
        return
    packer = Packer()

    if args['additional_path'] is not None:
        for path in args['additional_path']:
            packer.add_additional_include_path(str(Path(path).absolute()))

    packer.process_file(fileNameToProcess)
    packer.write_result_to_file(outFileName, False)

    os.chdir(filePath.parent)

if __name__ == "__main__":
    main()