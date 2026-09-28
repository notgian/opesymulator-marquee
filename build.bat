@echo off
cd /d "%~dp0"
if exist main.exe del /f /q main.exe
C:\MinGW\bin\g++.exe -std=c++17 -g *.cpp -o main.exe