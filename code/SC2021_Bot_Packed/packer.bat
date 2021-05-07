@echo off
set project-name=SC2021
set file-cpp-name=%project-name%Bot
python ../../external/one-cpp-packer/OneCppPacker.py -src ../%project-name%_Bot/%file-cpp-name%.cpp -dst %file-cpp-name%_Packed.cpp -addp "../"