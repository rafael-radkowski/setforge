#include "RandomImageGenerator.h"


/*
Constructor
@param image_height, image_width - the output image size in pixels. 
*/
RandomImageGenerator::RandomImageGenerator(int image_height, int image_widht) :
	_image_height(image_height), _image_widht(image_widht)
{
	_image_type = "";
	_render_path = "";
	_render_type = "";
	_output_path = "./batch";
	_output_file_name = "render_log.csv";

	_rendering_height = image_height;
	_rendering_widht = image_widht;

	_with_chromatic_adpat = false;
	_wtih_noise_adapt = false;
	_noise_sigma = 0.1;
	_noise_mean = 0.0;
}

RandomImageGenerator::~RandomImageGenerator()
{


}

/*
Set the path to the folder containing the background images.
@param image_path - vector of strings with the path to all files.
@param type - the image type to be read.
*/
void RandomImageGenerator::setImagePath(vector <string> image_paths, string type)
{
    _image_path = image_paths;
    _image_type = type;
}

/*
Set the path to the folder containing the rendered images along with the pose. 
@param image_path - strings with the path to all files.
@param type - the image type to be read.
*/
void RandomImageGenerator::setRenderPath(string render_path, string type)
{
    _render_path = render_path;
    _render_type = type;

}

/*
Set the output path and output type. 
@param path - string containing the path to all output imags
@param type - the output image type. 
*/
void RandomImageGenerator::setOutputPath(string path)
{
	_output_path = path;
	writeHeader();
}


/*
Set a filter and its parameters. The filter to be change is set with 'type'
The parameters param1 and param2 depend on the filter to be set.
@param type - a filter of type Filtertype, NOISE or CHROMATIC
@param enable - enable or disable this filter with true or false. Default is false. 
@param param1 - depends on the filter to set. 
			For NOISE: param1: sigma, param2: mean
			For CHROMATIC: no parameters
*/
void RandomImageGenerator::setFilter(Filtertype type, bool enable, float param1, float param2)
{
	switch (type) {
	case NOISE:
		_wtih_noise_adapt = enable;
		_noise_sigma = param1;
		_noise_mean = param2;
		break;
	case CHROMATIC:
		_with_chromatic_adpat = enable;
		break;
	}
}


/*
The function distinguises the "combine" mode and the "rendering only" mode using the 
image path string (setImagePath(...)). If the string is empty, the tool assues that 
only foreground renderings ought to be processed.
@param num_images - integer with the number of images to generate. 
@param with_background -  if true, the process will combine given background images randomly with foreground renderings.
@return - number of stored images
*/
int RandomImageGenerator::process(int num_images) {

	string path = _image_path[0];
	std::transform(path.begin(), path.end(), path.begin(), ::tolower);

	if ( path.compare("none") != 0) {
		return process_combine(num_images);
	}
	else {
		process_rendering();
	}

}

/*
Combine foreground images with rendering 
@param num_images - integer with the number of images to generate. 
@return - number of stored images
*/
int RandomImageGenerator::process_combine(int num_images)
{
    // read the background images
    image_filenames.clear();
	image_filenames = ReadImages::GetList(_image_path, _image_type);

	if (image_filenames.size() == 0) {
		cout << "[ERROR] - no images loaded" << endl;
		return 0;
	}
	else {
		cout << "[INFO] - Found " << image_filenames.size() << " images." << endl;
	}

	// read the rendered images
	rendered_files.clear();
	ImageLogReader::Read(_render_path, &rendered_files);

	if (rendered_files.size() == 0) {
		cout << "[ERROR] - no rendered images loaded" << endl;
		return 0;
	}
	else {
		cout << "[INFO] - Found " << rendered_files.size() << " images." << endl;
	}
 
    // generate random numbers
    int N = image_filenames.size();
    int M = rendered_files.size();

    //std::default_random_engine generator;
    std::random_device generator;
    std::uniform_int_distribution<int> distribution(0,N-1);
    std::uniform_int_distribution<int> distribution_render(0,M-1);
    int dice_roll = distribution(generator); 

    int backup_i = 0; // prevents deadlocks
    int i=0;
	cout << "\n[INFO] - Start to generate " << num_images << " images." << endl;
    while(i<num_images){
        backup_i++;
        if(backup_i > num_images*3) break;

        int dice_image = distribution(generator); 
        int dice_rendering = distribution_render(generator);
        //cout << dice_image << " : " << image_filenames[dice_image] << "\n";

		// Get the image paths. 
        string path0 = image_filenames[dice_image];
        string path1 = rendered_files[dice_rendering].rgb_file;
		string path2 = rendered_files[dice_rendering].normal_file;
		string path3 = rendered_files[dice_rendering].depth_file;
		string path4 = rendered_files[dice_rendering].maske_file;

		//----------------------------------------------
		// Read the background image and check if its ok.
        cv::Mat img = cv::imread(path0);
		if(img.rows == 0||img.cols == 0){
			std::cout << "[ERROR] - Did not find image " << path0 << ". Check the path." << std::endl;
		}

        int r = img.rows;
        int c = img.cols;
        
        if(r < int(_image_height / 2) || c < int(_image_widht / 2) ) continue; // image too tiny

        cv::Mat img_resized = adaptImage(img);

		//----------------------------------------------
		// process renderer images

        cv::Mat rendering = cv::imread(path1);
		if(rendering.rows == 0||rendering.cols == 0){
			std::cout << "[ERROR] - Did not find image " << path1 << ". Check the path." << std::endl;
		}

        int roi_x, roi_y, roi_width, roi_height;
        cv::Mat rendered_image = adaptRendering(rendering, roi_x, roi_y, roi_width, roi_height);
        //cout << roi_x << " : " << roi_y << "\n";


		//----------------------------------------------
		// Chromatic adaptation and noise filtering
		if (_with_chromatic_adpat) {
			_imageFilter.setChromaticTemplate(img_resized);
			_imageFilter.apply(rendered_image, rendered_image);
		}

		if (_wtih_noise_adapt) {
			rendered_image = NoiseFilter::AddGaussianNoise(rendered_image, _noise_mean, _noise_sigma);
		}


		//----------------------------------------------
		// Combine foreground with background

        cv::Mat ready_rgb = combineImages(img_resized, rendered_image, 0.0);
		cv::Mat output = ready_rgb.clone();
		cv::rectangle( output, cv::Point(roi_x, roi_y), cv::Point(roi_x + roi_width, roi_y + roi_height), cv::Scalar(255,0,0));
		

		//-----------------------------------------------------------------------------
		// normal processing

		// calculate the normal map
		cv::Mat img_normals;
		NormalMapSobel::EstimateNormalMap(img_resized, img_normals, 3, 25);

		// process normal image
		cv::Mat rendering_normals = cv::imread(path2, cv::IMREAD_ANYDEPTH | cv::IMREAD_UNCHANGED); // 16UC3
		if(rendering_normals.rows == 0||rendering_normals.cols == 0){
			std::cout << "[ERROR] - Did not find normal map " << path2 << ". Check the path." << std::endl;
		}
		cv::Mat rendering_normals_32F;
		rendering_normals.convertTo(rendering_normals_32F, CV_32FC3, 1.0/65534.0);

	//	cout << MatHelpers::Type2str(rendering_normals.type()) << endl;
	//	cv::imshow("in_normals", rendering_normals );
	//	cv::imshow("out_normals", rendering_normals_32F );
    //    cv::waitKey();


        int r_n = rendering_normals.rows;
        int c_n = rendering_normals.cols;
		cv::Mat rendered_normals2;
		cv::resize(rendering_normals_32F, rendered_normals2, cv::Size(_rendering_height, _rendering_widht ));
		cv::Mat ready_normals = combineNormals(img_normals, rendered_normals2, rendered_image,  0);
		

		//-----------------------------------------------------------------------------
		// Read and resize depth file
		cv::Mat ready_depth;
		cv::Mat img_depth = cv::imread(path3,  cv::IMREAD_UNCHANGED | cv::IMREAD_ANYDEPTH); // 16UC3
		if(img_depth.rows == 0||img_depth.cols == 0){
			std::cout << "[ERROR] - Did not find the depth image " << path3 << ". Check the path." << std::endl;
		}else
			cv::resize(img_depth, ready_depth, cv::Size(_rendering_height, _rendering_widht ));


		//-----------------------------------------------------------------------------
		// Read and resize mask file
		cv::Mat ready_mask;
		cv::Mat img_mask = cv::imread(path4,  cv::IMREAD_UNCHANGED | cv::IMREAD_ANYDEPTH); // 16UC3
		if(img_mask.rows == 0||img_mask.cols == 0){
			std::cout << "[ERROR] - Did not find the depth image " << path3 << ". Check the path." << std::endl;
		}else
			cv::resize(img_mask, ready_mask, cv::Size(_rendering_height, _rendering_widht ));


		//-----------------------------------------------------------------------------
		// write data to file


		writeDataEx(i, ready_rgb, ready_normals, ready_depth, ready_mask, rendered_files[dice_rendering], cv::Rect(roi_x, roi_y, roi_width, roi_height));

        cv::imshow("out",output );
		cv::Mat img_normals_out;
		//cv::cvtColor(img_normals, img_normals_out, cv::COLOR_RGB2BGR);
		cv::imshow("out_normals", ready_normals );
        cv::waitKey(1);
		//Sleep(0.015);
        i++;

		// progress ticker
		if (i > 1 && i % 100 == 0) {
			cout << ". ";
			if (i % 1000 == 0) {
				cout << " [" <<i << "/"<< num_images<< "]\n";
			}
		}
    }

   // cout << "[INFO] - Created " << i << " images." << endl;

    return i;
}


/*
Just renders the fprground image. No background image added
@param num_images - integer with the number of images to generate. 
@return - number of stored images
*/
int RandomImageGenerator::process_rendering(void)
{
	// read the rendered images
	rendered_files.clear();
	ImageLogReader::Read(_render_path, &rendered_files);

	if (rendered_files.size() == 0) {
		cout << "[ERROR] - no rendered images loaded" << endl;
		return 0;
	}
	else {
		cout << "[INFO] - Found " << rendered_files.size() << " images." << endl;
	}
 
    // generate random numbers
    int M = rendered_files.size();
	int num_images = M;

    //std::default_random_engine generator;
    std::random_device generator;
    std::uniform_int_distribution<int> distribution_render(0,M-1);

    int backup_i = 0; // prevents deadlocks
    int i=0;


	 while(i<num_images){
        backup_i++;
        if(backup_i > num_images*3) break;

        int dice_rendering = distribution_render(generator);
        string path1 = rendered_files[dice_rendering].rgb_file;
		string path2 = rendered_files[dice_rendering].normal_file;

		//----------------------------------------------
		// process renderer images
        cv::Mat rendering = cv::imread(path1);
		if(rendering.rows == 0||rendering.cols == 0){
			std::cout << "[ERROR] - Did not find image " << path1 << ". Check the path." << std::endl;
		}
        int roi_x, roi_y, roi_width, roi_height;
        cv::Mat ready_rgb = adaptRendering(rendering, roi_x, roi_y, roi_width, roi_height);
		cv::Mat output = ready_rgb.clone();
		cv::rectangle( output, cv::Point(roi_x, roi_y), cv::Point(roi_x + roi_width, roi_y + roi_height), cv::Scalar(255,0,0));
		

		//-----------------------------------------------------------------------------
		// normal processing
		// process normal image
		cv::Mat rendering_normals = cv::imread(path2, cv::IMREAD_ANYDEPTH | cv::IMREAD_UNCHANGED); // 16UC1
		if(rendering_normals.rows == 0||rendering_normals.cols == 0){
			std::cout << "[ERROR] - Did not find normal map " << path2 << ". Check the path." << std::endl;
		}
		cv::Mat rendering_normals_32F;
		rendering_normals.convertTo(rendering_normals_32F, CV_32FC3, 1.0/65534.0);

        int r_n = rendering_normals.rows;
        int c_n = rendering_normals.cols;
		cv::Mat rendered_normals2;
		cv::resize(rendering_normals_32F, rendered_normals2, cv::Size(_rendering_height, _rendering_widht ));
		cv::Mat ready_normals = rendered_normals2;
		
		//-----------------------------------------------------------------------------
		// write data to file


		writeData(i, ready_rgb, ready_normals, rendered_files[dice_rendering], cv::Rect(roi_x, roi_y, roi_width, roi_height));

        cv::imshow("out",output );
		cv::Mat img_normals_out;
		//cv::cvtColor(img_normals, img_normals_out, cv::COLOR_RGB2BGR);
		cv::imshow("out_normals", ready_normals );
        cv::waitKey(1);
		//Sleep(0.015);
        i++;
    }

    cout << "[INFO] - Created " << i << " images." << endl;

    return i;
}


 /*
Adapt the loaded image to geometric constraints. 
*/
cv::Mat RandomImageGenerator::adaptImage(cv::Mat& image)
{
    static float aspect = float(_image_height) / float(_image_widht);

    int r = image.rows;
    int c = image.cols;
    float a = float(r)/float(c);

    cv::Mat result;
    if(aspect == a){
         cv::resize(image, result, cv::Size(_image_height, _image_widht ));
    }
    else if(c >= _image_height && r >= _image_widht )
    {
        cv::Range rows(0, _image_height);
        cv::Range cols(0, _image_widht);
        result = image(rows, cols);
    }
    else
    {
         cv::resize(image, result, cv::Size(_image_height, _image_widht ));
    }

    return result;
}



cv::Mat RandomImageGenerator::adaptRendering(cv::Mat& image, int& x, int& y, int& width, int& height)
{
    static float aspect = float(_rendering_height) / float(_rendering_widht);
    int r = image.rows;
    int c = image.cols;
    
    float a = float(r)/float(c);

    cv::Mat result;
    cv::resize(image, result, cv::Size(_rendering_height, _rendering_widht ));
    

    //cv::imshow("searrch",result );
   // cv::waitKey(1);

    // find roi
    x = _rendering_widht;
    y = _rendering_height;
    width = 1;
    height = 1;

    cv::Vec3b last_pixel(0,0,0);
	int xr = 0;
	int yd = 0;

    for(int i =0; i<result.rows; i++){
        for(int j =0; j<result.cols; j++){
            cv::Vec3b col = result.at<cv::Vec3b>(i,j);
            // search for the first not 0 pixel
            bool all_zero =false;
            if(col[0] > 0 || col[1] > 0 || col[2] > 0){
                all_zero = true;
                if(j < x){
                    x = j;
                }
                if(i < y){
                    y = i;
                }
            }
            // search for the last not zero pixel
            if(last_pixel[0] > 0 || last_pixel[1] > 0 || last_pixel[2] > 0){
                if(all_zero == true){
                    if(j > xr){
						xr = j;
                    }
                    if(i > height + y){
                        height = i - y;
                    }
                }
            }

			


            last_pixel = col;
        }
		
    }
	width = xr - x;

    x = max((int)0, min((int)x, (int)_rendering_widht));
    y = max((int)0, min((int)y, (int)_rendering_height));

    width = max(1, min(width, _rendering_widht));
    height = max(1, min(height, _rendering_height));

    return result;
}



cv::Mat RandomImageGenerator::combineImages(cv::Mat image1, cv::Mat image2, int threshold_value)
{

	// find the rendering first by assuming that the background is black. 
    cv::Mat img, mask, grayscaleMat;
    img = image2.clone();
    cvtColor(img, grayscaleMat, CV_RGB2GRAY);
    cv::threshold(grayscaleMat, mask, threshold_value, 255, CV_THRESH_BINARY);

    cv::Mat result, mask_inv;
    cv::bitwise_not(mask, mask_inv);
    cv::bitwise_or(image1, image1, result, mask_inv );
    cv::bitwise_or(result, image2, result, mask );


    //cv::imshow("th",mask );
    //cv::waitKey(1);

    return result;
}


cv::Mat RandomImageGenerator::combineNormals(cv::Mat image1, cv::Mat image2, cv::Mat image_rgb, int threshold_value)
{
    cv::Mat img, mask, grayscaleMat;
    img = image_rgb.clone();
    cvtColor(img, grayscaleMat, CV_RGB2GRAY);
    cv::threshold(grayscaleMat, mask, threshold_value, 255, CV_THRESH_BINARY);
    cv::Mat result, mask_inv;
    cv::bitwise_not(mask, mask_inv);
    cv::bitwise_or(image1, image1, result, mask_inv );
    cv::bitwise_or(result, image2, result, mask );
	//cout << MatHelpers::Type2str(image2.type()) << endl;
	//cout << MatHelpers::Type2str(image1.type()) << endl;
    //cv::imshow("th",mask );
    //cv::waitKey(1);

    return result;
}



bool RandomImageGenerator::writeHeader(void)
{

	string out = _output_path;
	out.append("/");
	out.append(_output_file_name);

	bool ret = std::experimental::filesystem::exists(_output_path);

	if (ret)
	{
		// delete the log file if one exist 
		if (std::experimental::filesystem::exists(out))
			std::experimental::filesystem::remove(out);
	}


	if (!ret) {

		std::experimental::filesystem::create_directory(_output_path);
	}


	

	// write header
	std::ofstream of(out, std::ifstream::out | std::ifstream::app);
	if (of.is_open()){
		of << "index,rgb_file,normals_file,depth_file,mask_file,mat_file,tx,ty,tz,qx,qy,qz,qw,roi_x,roi_y,roi_w,roi_h\n";
	}
	of.close();


	return true;
}


bool RandomImageGenerator::writeData(int id,  cv::Mat& image_rgb, cv::Mat& image_normal, ImageLogReader::ImageLog& data, cv::Rect& roi)
{

	//----------------------------------------------
	// RGB image

	string name = _output_path;
	name.append("/");
	name.append(to_string(id));
	name.append("-");
	name.append(data.rgb_file);
		
	// removes the path name if the original file name comes with a path
	int index = data.rgb_file.find_last_of("/");
	if (index != -1) {
		string sub = data.rgb_file.substr(index + 1, data.rgb_file.length() - index - 1);

		name = _output_path;
		name.append("/");
		name.append(to_string(id));
		name.append("-");
		name.append(sub);

	}

	cv::imwrite(name, image_rgb);

	//----------------------------------------------
	// Normal image

	string name_d = _output_path;
	name_d.append("/");
	name_d.append(to_string(id));
	name_d.append("-");
	name_d.append(data.normal_file);
	// removes the path name if the original file name comes with a path
	index = data.rgb_file.find_last_of("/");
	if (index != -1) {
		string sub = data.normal_file.substr(index + 1, data.normal_file.length() - index - 1);

		name_d = _output_path;
		name_d.append("/");
		name_d.append(to_string(id));
		name_d.append("-");
		name_d.append(sub);

	}

	cv::Mat normals_16UC3;
	image_normal.convertTo(normals_16UC3, CV_16UC3, 65535 );
	cv::imwrite(name_d, normals_16UC3);

	//----------------------------------------------
	// Log file

	string out = _output_path;
	out.append("/");
	out.append(_output_file_name);
	std::ofstream of(out, std::ifstream::out | std::ifstream::app);

	if (of.is_open()) {
		of << id << "," << name << "," << name_d << "," << data.depth_file << "," << data.matrix_file  << "," << data.p.x << "," << data.p.y << "," << data.p.z << "," << data.q.x << "," << data.q.y << "," << data.q.z << "," << data.q.w << "," << roi.x << ',' << roi.y << "," << roi.width << "," << roi.height << "\n";
	}
	of.close();

	return true;
}



bool RandomImageGenerator::writeDataEx(int id, cv::Mat& image_rgb, cv::Mat& image_normal, cv::Mat& image_depth, cv::Mat& image_mask, ImageLogReader::ImageLog& data, cv::Rect& roi)
{
	//----------------------------------------------
	// RGB image

	string name = _output_path;
	name.append("/");
	name.append(to_string(id));
	name.append("-");
	name.append(data.rgb_file);
		
	// removes the path name if the original file name comes with a path
	int index = data.rgb_file.find_last_of("/");
	if (index != -1) {
		string sub = data.rgb_file.substr(index + 1, data.rgb_file.length() - index - 1);

		name = _output_path;
		name.append("/");
		name.append(to_string(id));
		name.append("-");
		name.append(sub);

	}

	cv::imwrite(name, image_rgb);

	//----------------------------------------------
	// Normal image

	string name_d = _output_path;
	name_d.append("/");
	name_d.append(to_string(id));
	name_d.append("-");
	name_d.append(data.normal_file);
	// removes the path name if the original file name comes with a path
	index = data.normal_file.find_last_of("/");
	if (index != -1) {
		string sub = data.normal_file.substr(index + 1, data.normal_file.length() - index - 1);

		name_d = _output_path;
		name_d.append("/");
		name_d.append(to_string(id));
		name_d.append("-");
		name_d.append(sub);

	}

	cv::Mat normals_16UC3;
	image_normal.convertTo(normals_16UC3, CV_16UC3, 65535 );
	cv::imwrite(name_d, normals_16UC3);


	//----------------------------------------------
	// depth image

	string name_de = _output_path;
	name_de.append("/");
	name_de.append(to_string(id));
	name_de.append("-");
	name_de.append(data.depth_file);
	// removes the path name if the original file name comes with a path
	index = data.depth_file.find_last_of("/");
	if (index != -1) {
		string sub = data.depth_file.substr(index + 1, data.depth_file.length() - index - 1);

		name_de = _output_path;
		name_de.append("/");
		name_de.append(to_string(id));
		name_de.append("-");
		name_de.append(sub);

	}

	cv::imwrite(name_de, image_depth);//CV_16UC1
	//cout << image_depth.type() << ", ";


	//----------------------------------------------
	// mask image

	string name_m = _output_path;
	name_m.append("/");
	name_m.append(to_string(id));
	name_m.append("-");
	name_m.append(data.maske_file);
	// removes the path name if the original file name comes with a path
	index = data.maske_file.find_last_of("/");
	if (index != -1) {
		string sub = data.maske_file.substr(index + 1, data.maske_file.length() - index - 1);

		name_m = _output_path;
		name_m.append("/");
		name_m.append(to_string(id));
		name_m.append("-");
		name_m.append(sub);

	}

	cv::imwrite(name_m, image_mask);//CV_16UC1
	//cout << image_mask.type() << "\n";

	//----------------------------------------------
	// Log file

	string out = _output_path;
	out.append("/");
	out.append(_output_file_name);
	std::ofstream of(out, std::ifstream::out | std::ifstream::app);

	if (of.is_open()) {
		of << id << "," << name << "," << name_d << "," << name_de << "," << name_m << "," << data.matrix_file  << "," << data.p.x << "," << data.p.y << "," << data.p.z << "," << data.q.x << "," << data.q.y << "," << data.q.z << "," << data.q.w << "," << roi.x << ',' << roi.y << "," << roi.width << "," << roi.height << "\n";
	}
	of.close();

	return true;
}