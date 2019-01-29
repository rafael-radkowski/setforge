#include "ReadImages.h"



 vector<string>  ReadImages::GetList(string path_to_image, string type)
 {
    vector<string>  files = getFileList(path_to_image );
    vector<string>  final_list;

    size_t s = files.size();

    transform(type.begin(), type.end(), type.begin(), ::tolower); 

    for(size_t i=0; i<s; i++){
        int index = files[i].find_last_of(".");
        if(index != -1){
            string type_file = files[i].substr(index+1, files[i].length() - index+1);
            transform(type_file.begin(), type_file.end(), type_file.begin(), ::tolower); 
            if(type_file.compare(type) == 0){
                final_list.push_back(files[i]);
            }
        }

    }
    return final_list;
 }


/*
Get all files of a specific type from a folder. It can read the files from multiple folders
@param path_to_image - a vector containing string with the relative or absolute path to folder. 
@param type - the image type. 
@return - vector containing string to all files. 
*/
//static 
vector<string>  ReadImages::GetList(vector<string> path_to_image, string type)
{
    vector<string>  final_list;

    for(int i=0; i<path_to_image.size(); i++){
        vector<string> files = ReadImages::GetList(path_to_image[i],  type);

        final_list.insert( final_list.end(), files.begin(), files.end() );

    }

    return final_list;
}



 /*
Read all files from a folder
*/
vector<string>  ReadImages::getFileList(string path)
{
    vector<string>  files;

#ifdef _WIN32
	std::experimental::filesystem::directory_iterator itr(path);

	for (const auto & entry : itr) {
		//std::cout << entry.path().string() << std::endl;
		files.push_back(entry.path().string());
	}

#else
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            //printf ("%s\n", ent->d_name);
            string f = path;
            f.append("/");
            f.append(ent->d_name);
            files.push_back(f);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        cout << "[ERROR] - Could not find folder " << path << "." << endl;
    }
#endif

    return files;
}