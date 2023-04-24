#!/bin/bash

printf "1\nclient1\n" | ../main_client localhost 7777 && printf "1\nclient2\n" | ../main_client localhost 7777 && printf "1\nclient3\n" | ../main_client localhost 7777 && printf "1\nclient4\n" | ../main_client localhost 7777 