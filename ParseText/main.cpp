#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map> 
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace std; 
using namespace cv;

// https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
vector<string> split(string str, string token){
    vector<string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

int main()
{


    string nvm_path = "../../Skydeploy/camera.nvm"; 
    string input_im_folder = "../../Skydeploy/images";

    cout << "Parsing nvm file : " << nvm_path << "\n";

    auto f = ifstream(nvm_path);

    map<int, string> im_mapping; 
    map<int, vector<vector<int>>> boxes_per_im;
    vector<int> id_vals = {}; 

    string line;

    int ctr = 1;
    while (getline(f, line))
    {
        vector<string> line_split;
        string s; 
        string split_token = " ";
        line_split = split(line, split_token);

        string im_name = line_split[0];

        if (im_name.find(".JPG") != string::npos || im_name.find(".jpg") != string::npos)
        {
            // im_mapping[ctr] = line_split[0]; 

            im_name = im_name.substr(0, min(im_name.find("jpg"), im_name.find("JPG"))+3);

            im_mapping[ctr] = im_name;
            id_vals.push_back(ctr);
            // cout << line_split[1] << "\n";

            ctr += 1;
        }
    }

    string text_path = "../../Skydeploy_Points.txt"; 
    string output_im_folder = "SkydeployBoxedImages";
    string output_mask_folder = "../../SkydeployBoxedMasks";

    cout << "Loading text file" << "\n";

    f = ifstream(text_path);

    // cout << std::count(std::istreambuf_iterator<char>(f), 
    //          std::istreambuf_iterator<char>(), '\n');

    cout << "Parsing text file" << "\n";

    // int max_row = 0;

    int counter = 0;

    while (getline(f, line))
    {
        vector<string> line_split;
        string s; 
        string split_token = ",";
        line_split = split(line, split_token);

        int id = stoi(line_split[0]);
        int x = stoi(line_split[2]);
        int y = stoi(line_split[1]);

        // if (id == 23)
            // max_row = max(max_row, y);

        if (id == 0) continue;

        if (boxes_per_im.find(id) == boxes_per_im.end())
            boxes_per_im[id] = {}; 
        else
            boxes_per_im[id].push_back({x, y});

        counter++;
        // if (counter > 10000000) break; 
    }

    // cout << "Max Row : " << max_row << "\n";

    cout << "Ceating texture masks " << "\n";

    for (int i = 0; i < id_vals.size(); i++)
    {
        // cout << i << "\n";


    // for (auto mapping: boxes_per_im)
    // {
        // cout << "FIRST" << "\n";

        int id = id_vals[i];

        // cout << "ID: " << id << "\n";
        
        // cout << "Grasping info" << "\n"; 
        auto infos = boxes_per_im[id];
        // cout << "Done grasping info" << "\n"; 

        string im_name = im_mapping[id];
        string im_path = input_im_folder + "/" + im_name;
        string output_im_path = output_mask_folder + "/" + im_name.substr(0, im_name.find(".")) + ".png"; 

        // if (im_path.find("IMG_9334") == string::npos)
            // continue;

        // cout << im_path << "\n";

        // if (im_path != "../../Skydeploy/images/S1012573.JPG") continue;

        // cout << output_im_path << "\n";

        // cout << id << "\n";/

        // int max_row = 0;
        // int max_col = 0;

        Mat curr_im = imread(im_path);

        // cout << curr_im.rows << "\n";
        // cout << "Rows picked up " << "\n";

        Mat mask(curr_im.rows, curr_im.cols, CV_8UC3, Scalar(0.0, 0.0, 0.0));

        // cout << "PRE FOR LOOP" << "\n";
        for (auto info: infos)
        {
            // cout << info.size() << "\n";
            
            int x = info[0];

            // cout << x << "\n";

            int y = info[1];

            // cout << y << "\n";

            // cout << x << "," << y << "\n";
            // cout << curr_im.rows << "," << curr_im.cols << "\n";

            // cout << "PRE VAL" << "\n";

            // cout << mask.rows << "," << mask.cols << "\n";

            auto& val = mask.at<Vec3f>(y, x);

            // cout << "VAL SIZE " << val.size() << "\n";

            val[0] = 255.0;
            val[1] = 255.0;
            val[2] = 255.0;

            // cout << "Setting val " << "\n";

            // val = 1.0; 

            // cout << val << "\n";

            // max_col = max(x, max_col);
            // max_row = max(y, max_row);

        }

        // cout << "POST FOR LOOP" << "\n";

        // cout << "Max Row : " << max_row << "\n";
        // cout << "Max Col : " << max_col << "\n";

        // cout << "Starting to write " << "\n";

        imwrite(output_im_path, mask);

        // cout << "Finished to write " << "\n";
        

    }

    // cout << "ready to return " << "\n";

    return 0;
}