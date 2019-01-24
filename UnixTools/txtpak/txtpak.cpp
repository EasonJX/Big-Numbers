#include "StdAfx.h"
#include "Library.h"

static void printBanner() {
  static bool done = false;
  if(!done) {
    _ftprintf(stderr, _T(" Txtpak Version 1.7  Copyright (c) 1999-2019 Jesper G. Mikkelsen\n\n"));
  }
  done = true;
}

typedef enum {
  NO_COMMAND
 ,CMD_UNPACK
 ,CMD_PACK
 ,CMD_LIST
 ,CMD_CHECK
} CommandType;

static void recurseSubDir(const String &dir, const String &pattern, StringArray &result) {
  DirList list = scandir(childName(dir, pattern), SELECTFILE);
  for(size_t i = 0; i < list.size(); i++) {
    result.add(childName(dir,list[i].name));
  }
  list = getAllSubDir(dir);
  for(size_t i = 0 ; i < list.size(); i++) {
    recurseSubDir(childName(dir, list[i].name), pattern, result);
  }
}

static StringArray findFiles(const TCHAR **argv, bool recurse) {
  StringArray result;
  if(!recurse) {
    for(;*argv; argv++) {
      const DirList list = scandir(*argv);
      for(size_t i = 0; i < list.size(); i++) {
        result.add(list[i].name);
      }
    }
  } else {
    for(;*argv; argv++) {
      recurseSubDir(_T("."), *argv, result);
    }
    result.sort(alphasort);
  }
  return result;
}

static StringArray readFileNames(const String &fileName) {
  String line;
  StringArray result;
  FILE *input = fileName.length() == 0 ? stdin : FOPEN(fileName, _T("r"));
  while(readLine(input, line)) {
    result.add(trim(line));
  }
  if(input != stdin) {
    fclose(input);
  }
  return result;
}

static void usage() {
  printBanner();
  _ftprintf(stderr
           ,_T("Usage: txtpak a archive [modifiers] [files...]\n"
               "       txtpak e archive [modifiers] [files...]\n"
               "       txtpak l archive [modifiers] [files...]\n"
               "       txtpak d archdir [modifiers] [files...]\n"
               "       txtpak c archdir\n"
               "Commands     a : Archive files.\n"
               "             e : Extract files.\n"
               "             l : List contents of archive.\n"
               "             d : Deflate directory tree-structure into a single directory, saving original filenames/positions in text\n"
               "                 archdir\\archdir.txt. New temporary filenames will be assigned to all files <guid>, saving extensions\n"
               "                 and copied to archdir\\<guid>.ext. All types of files, including non-text files, can be copied with this command.\n"
               "                 Use command e to restore directory tree-structure from archdir, using filename-mapping in archdir\\archdir.txt\n"
               "                 to restore files original position and name\n"
               "             c : Check integrity of library.\n"
               "Modifiers : -s : Sort files by name\n"
               "            -w : Overwrite files when unpacking. Default is ask for overwrite.\n"
               "            -b : Don't print banner.\n"
               "            -t : Don't set creation-,modification- or access-time when unpacking.\n"
               "                 Default these 3 timestamps will be set the value specified in the archive.\n"
               "            -m : Don't set read/write mode when unpacking.\n"
               "                 Default the mode will be set to the value specified in the archive.\n"
               "            -v : Verbose.\n"
               "            -f[file]: file contains filenames to pack.\n"
               "                      If file if not specified, stdin is used. Input can be generated by grep -H\n"
               "                      Ex: grep -Hr addAttribute *.cpp | txtpak a files -f\n"
               "                      will pack all cpp-files (in current directory, and subdirectories), containing the text \"addAttribute\"\n")
           );
  exit(-1);
}

int _tmain(int argc, const TCHAR **argv) {
  const TCHAR *cp;
  CommandType  command      = NO_COMMAND;
  bool         recurse      = false;
  bool         setTimestamp = true;
  bool         setMode      = true;
  bool         showBanner   = true;
  bool         sorting      = false;
  bool         verbose      = false;
  LibType      libType      = LT_UNKNOWN;
  const TCHAR *fileName     = NULL;

  argv++;
  if(!*argv) {
    usage();
  }
  switch(**argv) {
  case 'a': command = CMD_PACK   ; libType = LT_TEXTFILE;    break;
  case 'e': command = CMD_UNPACK ; break;
  case 'l': command = CMD_LIST   ; break;
  case 'd': command = CMD_PACK   ; libType = LT_DIRWITHMAP;  break;
  case 'c': command = CMD_CHECK; break;
  default : usage();
  }
  argv++;

  if(!*argv) {
    usage();
  }
  const String libName = *argv;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'w': overwriteAll = true;  continue;
      case 's': sorting      = true;  continue;
      case 'b': showBanner   = false; continue;
      case 'r': recurse      = true;  continue;
      case 't': setTimestamp = false; continue;
      case 'm': setMode      = false; continue;
      case 'f':
        fileName     = cp+1;
        break;
      case 'v': verbose      = true;  continue;
      default : usage();
      }
      break;
    }
  }

  if(showBanner) {
    printBanner();
  }

  Library library(libName, libType);
  try {
    switch(command) {
    case CMD_UNPACK :
      library.unpack(argv, setTimestamp, setMode, verbose);
      break;
    case CMD_PACK   :
      { StringArray fileNames;
        if(fileName == NULL) {
          fileNames = findFiles(argv, recurse);
        } else {
          fileNames = readFileNames(fileName);
        }
        if(!canOverWrite(libName)) {
          break;
        }
        try {
          switch(libType) {
          case LT_TEXTFILE:
            if(Library::getLibType(libName) == LT_DIRWITHMAP) {
              Library::removeLib(libName);
            }
            break;
          case LT_DIRWITHMAP:
            Library::removeLib(libName);
            break;
          }
        } catch(Exception e) {
          // ignore
        }
        if(sorting) {
          fileNames.sort(alphasort);
        }
        library.pack(fileNames, verbose);
      }
      break;
    case CMD_LIST   :
      library.list(argv, sorting);
      break;
    case CMD_CHECK  :
      library.checkIntegrity();
      break;
    default         :
      usage();
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("\ntxtpak:%s\n"), e.what());
    return -1;
  }
  return 0;
}
