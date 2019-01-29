#include "RandomImageGenerator.h"


/*
Constructor
@param image_height, image_width - the output image size in pixels. 
*/
RandomImageGenerator::RandomImageGenerator(int image_height, int image_widht):
    _image_height(image_height),  _image_widht(image_widht)
{
    _image_type = "";
    _render_path = "";
    _render_type = "";
	_output_path = "./batch";
	_output_file_name = "batch.csv";

    _rendering_height = image_height; 
    _rendering_widht = image_widht;
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
Start processing
@return - number of stored images
*/
int RandomImageGenerator::process(int num_images)
{
    // read the background images
    image_filenames.clear();
    image_filenames = ReadImages::GetList(_image_path, _image_type);

    if(image_filenames.size() == 0){
        cout << "[ERROR] - no images loaded" << endl;
        return 0;
    }
    else{
        cout << "[INFO] - Found " << image_filenames.size() << " images." << endl;
    }

    // read the rendered images
    rendered_files.clear();
	ImageLogReader::Read(_render_path, &rendered_files);

     if(rendered_files.size() == 0){
        cout << "[ERROR] - no rendered images loaded" << endl;
        return 0;
    }
    else{
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
    while(i<num_images){
        backup_i++;
        if(backup_i > num_images*3) break;

        int dice_image = distribution(generator); 
        int dice_rendering = distribution_render(generator);
        //cout << dice_image << " : " << image_filenames[dice_image] << "\n";

        string path0 = image_filenames[dice_image];
        string path1 = rendered_files[dice_rendering].rgb_file;

        cv::Mat img = cv::imread(path0);
        int r = img.rows;
        int c = img.cols;
        
        if(r < int(_image_height / 2) || c < int(_image_widht / 2) ) continue; // image too tiny

        cv::Mat img_resized = adaptImage(img);

        cv::Mat rendering = cv::imread(path1);

        int roi_x, roi_y, roi_width, roi_height;
        cv::Mat rendered_image = adaptRendering(rendering, roi_x, roi_y, roi_width, roi_height);
        //cout << roi_x << " : " << roi_y << "\n";
       // cv::rectangle( rendered_image, cv::Point(roi_x, roi_y), cv::Point(roi_x + roi_width, roi_y + roi_height), cv::Scalar(255,0,0));
        cv::Mat ready = combineImages(img_resized, rendered_image, 10);
		cv::Mat output = ready.clone();
		cv::rectangle( output, cv::Point(roi_x, roi_y), cv::Point(roi_x + roi_width, roi_y + roi_height), cv::Scalar(255,0,0));
		
		
		writeData(i, ready, rendered_files[dice_rendering], cv::Rect(roi_x, roi_y, roi_width, roi_height));

        cv::imshow("out",output );
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
		of << "index,rgb_file,normals_file,depth_file,mat_file,tx,ty,tz,qx,qy,qz,qw,roi_x,dx,roi_y,dy\n";
	}
	of.close();


	return true;
}


bool RandomImageGenerator::writeData(int id, cv::Mat& image, ImageLogReader::ImageLog& data, cv::Rect& roi)
{

	

	string name = _output_path;
	name.append("/");
	name.append(to_string(id));
	name.append("-");
	name.append(data.rgb_file);
				
	int index = data.rgb_file.find_last_of("/");
	if (index != -1) {
		string sub = data.rgb_file.substr(index + 1, data.rgb_file.length() - index - 1);

		name = _output_path;
		name.append("/");
		name.append(to_string(id));
		name.append("-");
		name.append(sub);

	}

	cv::imwrite(name, image);


	string out = _output_path;
	out.append("/");
	out.append(_output_file_name);
	std::ofstream of(out, std::ifstream::out | std::ifstream::app);

	if (of.is_open()) {
		of << id << "," << name << "," << data.normal_file << "," << data.depth_file << "," << data.matrix_file  << "," << data.p.x << "," << data.p.y << "," << data.p.z << "," << data.q.x << "," << data.q.y << "," << data.q.z << "," << data.q.w << "," << roi.x << ',' << roi.y << "," << roi.width << "," << roi.height << "\n";
	}
	of.close();

	return true;
}