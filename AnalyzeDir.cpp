/// =========================================================================
/// Navjot Saran
/// 30011914
/// CPSC457 A2
/// Some portions written by pfederl@ucalgary.ca for CPSC457.
/// =========================================================================

#include "analyzeDir.h"

#include <cassert>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
// #include <errno.h>
// #include <cstring>
#include <unordered_map>
#include <algorithm>
#include <sstream>

using namespace std;
constexpr int MAX_WORD_SIZE = 1024;

long numOfDirs = 1;
long numOfFiles = 0;
long totalFileSize = 0;
long largestFileSize = -1;
string largestFilePath = "";
std::unordered_map<std::string,int> hist;
std::vector<ImageInfo> largestImages;
bool isVacant;
std::vector<std::string> vacantDirs;
string currentVacantPath;

/// check if path refers to a directory
static bool is_dir(const std::string & path)
{
  struct stat buff;
  if (0 != stat(path.c_str(), &buff)) return false;
  return S_ISDIR(buff.st_mode);
}

/// check if path refers to a file
static bool is_file(const std::string & path)
{
  struct stat buff;
  if (0 != stat(path.c_str(), &buff)) return false;
  return S_ISREG(buff.st_mode);
}

/// check if string ends with another string
static bool ends_with(const std::string & str, const std::string & suffix)
{
  if (str.size() < suffix.size()) return false;
  else
    return 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

/// check if string starts with another string
static bool starts_with(const std::string & str, const std::string & suffix)
{
  if (str.size() < suffix.size()) return false;
  else
    return 0 == str.compare(0, suffix.size(), suffix);
}

bool compareImgSizes(const ImageInfo &a, const ImageInfo &b) {
  return (a.width * a.height) > (b.width * b.height);
}

// Scans the directory and gets stats for results
void scan(const std::string & dir, int n)
{
  auto dirp = opendir( dir.c_str());
  assert(dirp != nullptr);
  for (auto de = readdir(dirp); de != nullptr; de = readdir(dirp)) {
    std::string name = de->d_name;
    if (name == "." or name == "..") continue;
    std::string path = dir + "/" + name;

    if (is_dir(path)) {
      numOfDirs++;
      isVacant = true;
    }

    if (is_file(path)) {
      numOfFiles++;
      isVacant = false;
      // cout << path << "is NOT vacant" << endl;
      currentVacantPath = "";
      // report size of the file
      {
      struct stat buff;
      if (0 != stat(path.c_str(), &buff)) {
        // std::cout << "Failed to get file stats: " << strerror(errno) << "\n";
      }
      else {
        // printf("    - file size: %ld bytes\n", long(buff.st_size));
        long currentFileSize = buff.st_size;
        totalFileSize = totalFileSize + currentFileSize;
        if(currentFileSize > largestFileSize) {
          largestFileSize = currentFileSize;
          largestFilePath = path;
        }
          // check if filename ends with .txt
        if (ends_with(path, ".txt")) {
          // printf("    - is a .txt file\n");
          FILE* file = std::fopen(path.c_str(), "r"); 
          if (file == nullptr) {
              std::perror("Error opening file");
              std::exit(EXIT_FAILURE);
          }

          // std::unordered_map<std::string,int> hist;
          while(1) {
          std::string result;
            while(1) {
              int c = fgetc(file);
              if(c == EOF) break;
              c = tolower(c);
              if(! isalpha(c)) {
                if(result.size() == 0)
                  continue;
                else
                  break;
              }
              else {
                if(result.size() >= MAX_WORD_SIZE) {
                  // printf("input exceeded %d word size, aborting...\n", MAX_WORD_SIZE);
                  exit(-1);
                }
                result.push_back(c);
              }
            }
            if(result.size() == 0) break;
            if(result.size() > 4) {
              hist[result] ++;
            }
          }
        }
        else {
          // printf("    - is not a .txt file\n");
          std::string cmd = "identify -format '%wx%h' " + path + " 2> /dev/null";
          // std::cout << "img info: " << cmd << std::endl;
          std::string img_size;
          auto fp = popen(cmd.c_str(), "r");
          assert(fp);
          char buff[PATH_MAX];
          if( fgets(buff, PATH_MAX, fp) != NULL) {
            img_size = buff;
          }
          int status = pclose(fp);
          if( status != 0 or img_size[0] == '0')
            img_size = "";
          if( img_size.empty()) {
            // printf("    - not an image\n");
          }
          else {
            string imgSize = img_size.c_str();
            std::stringstream ss(imgSize);
            long wid;
            long hei;
            ss >> wid;
            ss.ignore(); // ignore x
            ss >> hei;
            largestImages.push_back({ path, wid, hei });
            // std::cout << "LARGESTIMGZZ = " << largestImages[0].path << " " << largestImages[0].width << "X" << largestImages[0].height << " " << std::endl;
          }
        }
      }
      }
      continue;
    }
    assert(is_dir(path));
    // scan(path, n);
    // cout << "before rec: " << path << endl;
    if(isVacant == true) {
      // cout << path << " is vacant" << endl;

      std::vector<std::string> splitPath;
      size_t front = 0, back = 0;
      while ((back = path.find('/', front)) != std::string::npos) {
        splitPath.push_back(path.substr(front, back - front));
        front = back + 1;
      }
      splitPath.push_back(path.substr(front));
      string directoryName = "";
      if (splitPath.size() > 1) {
        directoryName = splitPath[1];
      } else {
        directoryName = ".";
      }
      // cout << "direname: " << directoryName << endl;
      // cout << "pathname: " << name << endl;
      if(directoryName == name) {
        vacantDirs.push_back(name);
      }
      currentVacantPath = path;
    }
    // cout << "vacant path BEFORE: " << currentVacantPath << endl;
    scan(path, n);
    // cout << "after rec: " << path << endl;
    // cout << "vacant path AFTER: " << currentVacantPath << endl;
  }
  closedir(dirp);
}

// ======================================================================
// You need to re-implement this function !!!!
// ======================================================================
//
// analyzeDir(n) computes stats about current directory
//   n = how many words and images to report in restuls
//
// The code below is incomplate/incorrect. Feel free to re-use any of it
// in your implementation, or delete all of it.
Results analyzeDir(int n)
{
  scan(".", n);
  if( starts_with(largestFilePath, "./"))
    largestFilePath = largestFilePath.substr(2);
  
  // first we place the words and counts into array (with count
  // negative to reverse the sort)
  // cout << "N VALUE IN MAIN MANE: " << n << endl;
  std::vector<std::pair<int,std::string>> arr;
  for(auto & h : hist)
    arr.emplace_back(-h.second, h.first);
  // if we have more than N entries, we'll sort partially, since
  // we only need the first N to be sorted
  if(arr.size() > size_t(n)) {
    std::partial_sort(arr.begin(), arr.begin() + n, arr.end());
    // drop all entries after the first n
    arr.resize(n);
  } else {
    std::sort(arr.begin(), arr.end());
  }
  
  if(largestImages.size() > size_t(n)) {
    std::partial_sort(largestImages.begin(), largestImages.begin() + n, largestImages.end(), compareImgSizes);    // drop all entries after the first n
    largestImages.resize(n);
  } 
  // else {
  //   std::sort(largestImages.end(), largestImages.begin(), compareImgSizes);
  // }
  // sort(largestImages.begin(), myvector.end());
  // // now we can print the n entries
  // printf("%d MCWMCWMCWMCMCW:\n", n);
  // for(auto & a : arr)
  //   printf("  - %s x %d\n", a.second.c_str(), -a.first);

  // the results below willl be used later to print. Some are still hardcoded their default values.
  Results res;
  res.largest_file_path = largestFilePath;
  res.largest_file_size = largestFileSize;
  res.n_files = numOfFiles;
  // cout << "result dircount: " << reslt.dirsCount << endl;
  res.n_dirs = numOfDirs;
  res.all_files_size = totalFileSize;

  for(auto & a : arr) {
    // printf("  - %s x %d\n", a.second.c_str(), -a.first);
    res.most_common_words.push_back({ a.second.c_str(), -a.first });
  }

  // for(auto & g : largestImages) {
  //   res.largest_images.push_back({ "img2.png", 200, 300 });
  // }

  for (const auto& f : largestImages) {
    res.largest_images.push_back({ f.path, f.width, f.height });
  }

  // ImageInfo info1 { "img1", 640, 480 };
  // res.largest_images.push_back(info1);
  // res.largest_images.push_back({ "img2.png", 200, 300 });

  // for (auto& k : vacantDirs) {
  //   res.vacant_dirs.push_back(k.);
  // }

  for (unsigned long i = 0; i < vacantDirs.size(); i++) {
    res.vacant_dirs.push_back(vacantDirs[i]);
  }
  // res.vacant_dirs.push_back("path1/subdir1");
  // res.vacant_dirs.push_back("test2/xyz");
  return res;
}