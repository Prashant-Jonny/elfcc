# ELFCC

The basic idea of this project is to create a file format allowing to easily 
manipulate ELF files. This file format is the elf source (elfs) and elfcc is 
the compiler and decompiler allowing to translate elf into elfs and vice versa.


This software doesn't have many uses and is mostly a tool to play around with 
the ELF file format. The main motivation behind this tool was for me to learn 
more about this format and its undocumented parts.

# Usage

To decompile an ELF into an elfs file and section dumps:

'''elfcc -d /bin/ls ./ls'''

An output base path can be provided as the last argument. If not, the ELF file 
is used as a base path.


To compile an elfs and seciton dumps into an ELF:

'''elfcc -c ./ls /bin/ls'''

An output ELF file can be provided as the last argument. If not, the base path 
is used as the ELF file name.

# License

Elfcc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Elfcc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Copyright 2014 Charles Hubain <haxelion@gmail.com>
