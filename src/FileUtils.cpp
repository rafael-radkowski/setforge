#include "FileUtils.h"


#ifdef _WIN32
	#if _MSC_VER >= 1920 && _MSVC_LANG  == 201703L 
		namespace fs = std::filesystem;
	#else
		namespace fs = std::experimental::filesystem;
	#endif
#endif



 /*
Check if a file exits. 
@param path_and_file - string containing the path and file, relative or absoulte. 
@return true, if the file exits. 
*/
//static 
bool FileUtils::Exists(string path_and_file)
{

#ifdef _WIN32
#if _MSC_VER >= 1920 && _MSVC_LANG  == 201703L 
	return std::filesystem::exists(path_and_file);
#else
	return std::experimental::filesystem::exists(path_and_file);
#endif
#else
    int res = access(path_and_file.c_str(), R_OK);
    if (res < 0) {
        if (errno == ENOENT) {
            // file does not exist
            return false;
        } else if (errno == EACCES) {
            // file exists but is not readable
            return false;
        } else {
             return false;
        }
    }
    return true;
#endif

}


/*
Search for a file in adjacent folders
@param path_and_file - relative or absolute location and name of the file.
@param new_path_and_file - relative or absolute location and name of the located file or "" if no file exists. 
@return true, if the file exists, false otherwise. 
*/
//static 
bool FileUtils::Search(string path_and_file, string& new_path_and_file)
{
	// Check if the file exists at this location. 
	if (Exists(path_and_file)) {
		new_path_and_file = path_and_file;
		return true;
	}

 #ifdef _WIN32
	// replace all \\ with /
	// ToDo: replace all \\ with /


	// Extract the filename from the path. 
	new_path_and_file = "";
	int idx = path_and_file.find_last_of("/");

	string file = "";
	if(idx == -1 ){
		file = path_and_file;
	}
	else {
		file = path_and_file.substr(idx+1, path_and_file.length() - idx -1);
	}

	// Identify the search paths. 
	std::list<string> search_path_list;
	search_path_list.push_back(".");
	search_path_list.push_back("../");
	search_path_list.push_back("../../");
	
	// traverse to all adjacent folders. 
	while(search_path_list.size() > 0){
		string e = search_path_list.front();
		search_path_list.pop_front();

#if _MSC_VER >= 1920 && _MSVC_LANG  == 201703L 
		for (const auto & entry : fs::directory_iterator(e)){
			if(entry.is_directory()){
				search_path_list.push_back(entry.path().string());
			}
		}
#else
		if( std::experimental::filesystem::is_directory(e)){
			search_path_list.push_back(e);
		}
#endif

		

		// search for all files in these folder. 
		// Return if the search path exists. 
		string p = e + "/" + file;
		if(Exists(p)){
			new_path_and_file = p;
			cout << "[INFO] - Changed path for " << path_and_file << " to " << new_path_and_file << "." << endl;
			return true;
		}

	}

	new_path_and_file = "";
	return false;
 #else
	// Linux version is not implemented. The code returns false if the file does not exists. 
	if (FileUtils::Exists(path_and_file)) {
		new_path_and_file = path_and_file;
		return true;
	}else{
		new_path_and_file = "";
		return false;
	}
 #endif
}





//static 
bool FileUtils::CreateDirectories(string path)
{
#if _MSC_VER >= 1920 && _MSVC_LANG  == 201703L 
	return std::filesystem::create_directories(path);
#else
	return std::experimental::filesystem::create_directories(path);
#endif
}


/*
Return the names of all files at the given path
@param path  - string containing the path
@return std::vector with the file names. 
*/
//static
vector<string> FileUtils::GetFileList(string path)
{
	std::vector<std::string> files;
#if _MSC_VER >= 1920 && _MSVC_LANG  == 201703L 
	std::filesystem::directory_iterator itr(path);

	for (const auto & entry : itr) {
		//std::cout << entry.path().string() << std::endl;
		files.push_back(entry.path().string());
	}
#else

	std::experimental::filesystem::directory_iterator itr(path);

	for (const auto & entry : itr) {
		//std::cout << entry.path().string() << std::endl;
		files.push_back(entry.path().string());
	}
#endif

	return files;
}


//static 
bool FileUtils::Remove(string path)
{
#if _MSC_VER >= 1920 && _MSVC_LANG  == 201703L 
	return std::filesystem::remove(path);
#else
	return std::experimental::filesystem::remove(path);
#endif
}


	
//static 
bool FileUtils::CreateDirectory(string path)
{
#if _MSC_VER >= 1920 && _MSVC_LANG  == 201703L 
	return std::filesystem::create_directory(path);
#else
	return std::experimental::filesystem::create_directory(path);
#endif

}