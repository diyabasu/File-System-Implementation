#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <cstdio>
using namespace std;

string get_path(string filename){
	filename.append(".txt");
//	string f = "\mount_point\";
//	f.append(filename);
	return filename;
}

// check if the file with given name exist or not
bool file_exist(map<int, string> mp, string filename){
	for(auto it = mp.begin(); it != mp.end(); ++it)
    {
      	string &value = it->second;
      	if(value == filename){
      		return true;
		}
    }
	return false;
}

// This function finds the inode number from filename
int get_inode(map<int, string> mp, string filename){
	for(auto it = mp.begin(); it != mp.end(); it++){
		string &value = it->second;
		if(value == filename){
			return it->first;
		}
	}
	return -1;
}

int main(int argc, char** argv) {
	
	// declaring variables
	string command;
	
	map<int, vector<int>> inode_blocklist;
	map<int, string> inode_filename;
	vector<int> all_blocks;
	
	// get started with the commands
	cout << "\n************** You are entering the basic file system **************" << endl;
	cout << "\n--------------------------------------------------------------------" << endl;
	cout << "Instructions ";
	cout << "\n--------------------------------------------------------------------";
	cout << "\nmf <<file-name>> \"filecontents\" - Use this command to create file";
	cout << "\ndf <<file-name>> - Use this command to delete file";
	cout << "\nrf <<old-file>> <<new-file>> - Use this command to rename file";
	cout << "\npf <<file-name>> - Use this command to display file content";
	cout << "\nls - Use this command to list all files";
	cout << "\nq - Use this command to quit";
	cout << "\n--------------------------------------------------------------------";
	cout << endl;
	while(true){
		cout << ">>";
		getline(cin, command);
		if(command.length() >= 2){
			//////////////////////////////////////////////////////////////////
			///////////////////////   Create File	//////////////////////////
			//////////////////////////////////////////////////////////////////
			if(command.rfind("mf", 0) == 0){
				// create file
				// divide the command
				stringstream ss(command);
				ss >> command;
				string filename;
				ss >> filename;
				// filename = get_path(filename);
				string content;
				getline(ss, content);

				// check if the file with same name exist or not
				if(file_exist(inode_filename, filename)){
					cout << "ERROR: Filename already exist. Try again!" << endl;
					continue;
				}
				
				// create a file with given name and content
				// add the file name to vector
				int inode = 10000 + rand() % 10000;
				while(inode_filename.find(inode) != inode_filename.end()){
					inode = 10000 + rand() % 10000;
				}
				inode_filename.insert({inode, filename});
				filename = to_string(inode);
				filename = get_path(filename);
				
				ofstream myfile(filename);
				for(int i=1; i<content.size(); i+=4){
					// get the block content
					string c = string(&content[i], &content[i+4]);
					
					// get the empty block
					int b = 1 + rand() % 9999;
					while(find(all_blocks.begin(), all_blocks.end(), b) != all_blocks.end()){
						b = 1 + rand() % 9999;
					}
					all_blocks.push_back(b);
					myfile << b << endl;
					inode_blocklist[inode].push_back(b);
										
					// create the file with block number
					string f = to_string(b);
					f = get_path(f);
					ofstream temp(f);
					
					// write block content to the file
					temp << c;
					temp.close();
				}		
				myfile.close();
				cout << "SUCCESS: File has been created successfully!" << endl;
			}
			else if(command.rfind("df", 0) == 0){
				// delete file from the command
				stringstream ss(command);
				ss >> command;
				string filename;
				ss >> filename;
				
				// get the inode number of file
				// filename = get_path(filename);
				int inode = get_inode(inode_filename, filename);
				// cout << "\nInode: " << inode;
				if(inode == -1){
					// file not found
					cout << "ERROR: No such file exist. Try again!" << endl;
					continue;
				}
				
				// delete all the block files 
				for(int x: inode_blocklist[inode]){
					string f = to_string(x);
					f = get_path(f);
					int result = remove(f.c_str());
				}
				
				// delete the inode file
				remove(get_path(to_string(inode)).c_str());
				
				// remove entry from vectors
				inode_blocklist.erase(inode);
				inode_filename.erase(inode);
				
				cout << "SUCCESS: The file has been deleted!" << endl;
				
			}
			else if(command.rfind("rf", 0) == 0){
				// rename file
				stringstream ss(command);
				ss >> command;
				string oldfilename;
				ss >> oldfilename;
				string newfilename;
				ss >> newfilename;
				
				// get the inode number of file
				int inode = get_inode(inode_filename, oldfilename);
				// cout << "\nInode: " << inode;
				if(inode == -1){
					// file not found
					cout << "ERROR: No such file exist. Try again!" << endl;
					continue;
				}
				
				// set the new file name
				inode_filename[inode] = newfilename;
				
				cout << "SUCCESS: The file has been renamed successfully!" << endl;
			}
			else if(command.rfind("pf", 0) == 0){
				// print file
				stringstream ss(command);
				ss >> command;
				string filename;
				ss >> filename;
				
				// get the inode number of file
				int inode = get_inode(inode_filename, filename);
				// cout << "\nInode: " << inode;
				if(inode == -1){
					// file not found
					cout << "ERROR: No such file exist. Try again!" << endl;
					continue;
				}
				
				// open the file with inode
				filename = get_path(to_string(inode));
				ifstream inodefile(filename);
				if(!inodefile.is_open()){
					cout << "ERROR: Can't open the file. Try again!" << endl;
				}
				
				// read block numbers from file
				string line;
				while (getline(inodefile,line))
			    {
			    	line = get_path(line);
			    	string blockline;
					ifstream blockfile(line);
					getline(blockfile, blockline);
					cout << blockline;
					blockfile.close(); 
			    }
				inodefile.close();
				cout << endl;
			}
			else if(command.rfind("ls", 0) == 0){
				// list files
				cout << "Filename\t\tInode" << endl;
				cout << "*************************************************" << endl;
				for(auto it=inode_filename.begin(); it!=inode_filename.end(); it++){
					cout << it->second << "\t\t\t" << it->first << endl;
				}
				cout << "*************************************************" << endl;
			}
			else{
				cout <<"ERROR: Invalid commnd" << endl;
			}
		}
		else{
			if(command.length() == 1){
				if(command[0] == 'q' || command[0] == 'Q'){
					cout << "\nExiting..." << endl;
					cout << "\n--------------------------------------------------------------------" << endl;
					cout << "THANK YOU" << endl;
					cout << "--------------------------------------------------------------------" << endl;
					return 0;
				}
			}
			else{
				cout <<"ERROR: Invalid commnd" << endl;
			}
		}
	}
	return 0;
}
