#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <cstdlib>
#include <bits/stdc++.h>

using namespace std;


long long binConv(string binNum)
{
	long long decimal = 0;
	int size = binNum.size();
	
	for (int counter = 0; counter <size; counter++)
	{
		if (binNum[counter] == '1')
			decimal = 10*decimal + 1;

		else
			decimal = 10*decimal;
		//decimal=10*decimal+binNum[counter]-'0';
	}
	return decimal;
}


int binaryToDecimal(long long n) 
{ 
	long long  num = n; 
	int dec_value = 0; 

	// Initializing base value to 1, i.e 2^0 
	int base = 1; 

	long long  temp = num; 
	while (temp) { 
		long long  last_digit = temp % 10; 
		temp = temp / 10; 

		dec_value += last_digit * base; 

		base = base * 2; 
	} 

	return dec_value; 
} 

int extractIntegerWords(string str , vector<int>&v) 
{ 
	stringstream ss;	 
	ss << str; 
	string temp,s; 
	int found; 
	while (!ss.eof()) { 
		ss >> temp; 
		if (stringstream(temp) >> found) {
				v.push_back(found);
			}
		temp = ""; 
	} 	
	return 0;
} 

int type_assign( map<string,string > & itype)
{
   string str1;
   string str2;
   ifstream file_type;
   file_type.open("type.txt");

  while(!file_type.eof())
  {
	  file_type >> str1;
	  file_type >> str2;
	  itype.insert(pair<string,string>(str1,str2)); 
  }
    file_type.close();
    return 0;
}

int binary_assign(map<string, string>& binary_f)
{
  string str1;
  string str2;
  ifstream file_type;
  file_type.open("binary_form.txt");

  while(!file_type.eof())
  {
	  file_type >> str1;
	  file_type >> str2;
	  binary_f.insert(pair<string,string>(str1,str2)); 
  }
    file_type.close();
    return 0;
}



int register_num_assign(map<string, string>&registers_num) 
{
  string str1;
  string str2;
  ifstream file_type;
  file_type.open("registers.txt");

  while(!file_type.eof())
  {
	  file_type >> str1;
	  file_type >> str2;
	  registers_num.insert(pair<string,string>(str1,str2)); 
  }
    file_type.close();
    return 0;
}     
  //int 5 bit binary value


string nibble_hex(string sin,int num_nibble)       //better usr it to convert only one nibble each time in loop
{
    int hex[1000];
    string s_out;
    for (int t = 0 ; t < num_nibble ; t++)
    {
	  string s_nibble;
	  int i = 1, j = 0, rem, dec = 0, binaryNumber=0;
	  for (int j =t*4 ; j < ((t+1)*4) ; j++)
	  {
	     	s_nibble=s_nibble+sin[j];
	  }
	    if(s_nibble == "0000")
 	     {	
 	     	s_out += "0";
 	     }
	  
	  
	  binaryNumber = atoi(s_nibble.c_str());

	  while (binaryNumber > 0) 
	  {
		   rem = binaryNumber % 2;
		   dec = dec + rem * i;
		   i = i * 2;
		   binaryNumber = binaryNumber / 10;
	  }
	  
	  i = 0;
	  while (dec != 0) 
	  {
		   hex[i] = dec % 16;
		   dec = dec / 16;
		   i++;
	  }
	  
	  for (j = i - 1 ; j >= 0 ; j--)
	  {
		   if (hex[j] > 9)  {
		    s_out += (char)(hex[j] + 55);
		   } 
		   else{
		    s_out += (char)(hex[j] + 48);
		   }
	  }
    }
 // cout<<endl<<endl<<s_out;
  return s_out;
}

int main(int argc, char const *argv[])
{

     map<string, string> registers_num;
	 map<string, string> itype;
	 map<string, string> binary_f;

     type_assign(itype);   				  //itype map have differnt instrunction as key and type as value
     binary_assign(binary_f);     		  //binary_f map have differnt instrunction as key and binary as value
     register_num_assign(registers_num);  //take resister name as input and give its nmber as output as x0=00000

    //intput instructions


	ofstream file_out;
	file_out.open("machine_code.mc");
	ifstream file_in2;
	file_in2.open("input.asm");	
	
	string inst_name,inst_name2, binary_inp, type, initial_bin, rs1, rs2, rd ,rsd;

	int line_address_text = -4;		 //.text start from 0 in memory ,-4 because +4 before assingment
	int line_address_text2 = -4;		 //.text start from 0 in memory ,-4 because +4 before assingment
	int line_address_data = 2000;		 //.data start from 2000 in memory

	//file_in>>s;
	int data = 0,data2=0, text = 0,text2=0, label_stage = 0 ,label_stage2=0;
	
     map <string,string> data_map; 
     map <string,string> data_label_map; //will contain address of 1st instuction after .word/.asciiz/.byte as value;
     map <string,string> text_label_map;   //will contain address of 1st instuction after it as value;
  
     string label_name;
     string label_name2;
	 bitset<32> addr32;


	while(!file_in2.eof())
	{
		int imm;
		 file_in2 >> inst_name2;
		 if( file_in2.eof() ) break;
	
		if(inst_name2 == ".data")
		{
			data2 =1;
			text2 = 0;	
		}
		else if(inst_name2 == ".text")
		{
			text2 =1;
			data2 = 0;
		}
		else if(data2==1)
		{
			string data_type2,str2;
			file_in2 >> data_type2;
			 if( file_in2.eof() ) break;

			 getline(file_in2,str2);
			 if( file_in2.eof() ) break;
		}
		else if(text2==1)
		{
			if( inst_name2[inst_name2.size()-1] == ':')
			{
				label_name2 = inst_name2;
				label_name2.erase(label_name2.size()-1);
				label_stage2 = 1;   //waiting for next instruction
				continue;
			}
			line_address_text2 += 4;
			if(label_stage2 == 1)
			{
				bitset<32>bset32;
				bset32 = line_address_text2;
				string b_l_a = bset32.to_string<char,std::string::traits_type,std::string::allocator_type>();			   
				text_label_map.insert(pair<string,string>(label_name2,b_l_a));
				label_stage2 = 0;
			}

			map<string, string>::iterator it = itype.find(inst_name2);
			if(it==itype.end())
			{
				cout<<"command "<<inst_name2<<" not found";
				return 0;
			}

			type = it->second;
			if( file_in2.eof() ) break;

			if(type == "R")
			{
			   	file_in2 >> rd >> rs1 >> rs2;
				 if( file_in2.eof() ) break;
			}

			else if(type == "I")
			{
				string s_bracket;
				if(inst_name2[0] == 'l'|| inst_name2[0] == 'j')
				{
					file_in2 >> rd >> s_bracket;
				    if( file_in2.eof() ) break;

					if(s_bracket[s_bracket.size()-1] == ')')	
					 {}
					else
					{
						line_address_text2 += 4;
					}
				}
			    else
			    {
				    file_in2 >> rd >> rs1 >> imm;
				   if( file_in2.eof() ) break;
				}
			}

			 else if(type == "U")
			{
			    string imm_str;
			    file_in2 >> rsd >> imm_str;
			    if( file_in2.eof() ) break;
			}

			 else if(type == "UJ")
			{
				string label_str;
			    file_in2 >> rsd >> label_str;
			     if( file_in2.eof() ) break;
			}

			 else if(type=="SB")
			{
				string label_str;
				file_in2>> rs1 >> rs2 >> label_str;
				 if( file_in2.eof() ) break;
			}

			 else if(type == "S")
			{
				string s_bracket;
			    file_in2 >> rs2 >> s_bracket;
			     if( file_in2.eof() ) break;
			}
			  else if(type=="E") {
		    	
			     if( file_in2.eof() ) break;
			    file_in2 >> rs2 ;
			     if( file_in2.eof() ) break;

	         }
       	}
       	else 
       	{
       		cout<<"Enter .data  or  .text ";
       		return 0;
       	}
 	}
 	
	
	file_in2.close();
	ifstream file_in;
	file_in.open("input.asm");

	while(!file_in.eof())
	{
		file_in >> inst_name;
		cout<< inst_name;
		 if( file_in.eof() ) break;
	
		if(inst_name == ".data")
		{
			data ++;
			text = 0;	
		}
		else if(inst_name == ".text")
		{
			text ++;
			data = 0;
			file_out<<"0xffffffff\t"<<"0xffffffff"<<endl;	
		}

		else if (data != 0)
		{
			string data_type;
			file_in >> data_type;
			 if( file_in.eof() ) break;
										///to resolve the situation map with key as label 
										///and all address as value in vector
			if(data_type == ".asciiz")
			{
				string asciiz;
				file_in >> asciiz;
				if( file_in.eof() ) break;
				
				bitset<8> wrd8;
				for (int i = 0; i < asciiz.size(); ++i)
				{
					addr32 = line_address_data;
	                string d = addr32.to_string<char,std::string::traits_type,std::string::allocator_type>();
			    	if(i == 0)
			    	{
			    		inst_name.erase(inst_name.size()-1);
			    		data_label_map.insert(pair<string,string>(inst_name,d));
			    	}	
				    d = nibble_hex(d,8);				    			    	
				    d="0x"+d;
			    	file_out << d << "\t";
			    	
			    	int ch = (int) asciiz[i];
                	wrd8   = ch;
                	string h = wrd8.to_string<char,std::string::traits_type,std::string::allocator_type>();
			    	h = nibble_hex(h,2);
				    h="0x"+h;
			    	file_out << h<<endl;

			    	
			    	data_map.insert(pair<string,string>(d,h));//d address; h is data
		    		line_address_data += 1;
		    	}				                  
			}

			if(data_type == ".word")
			{
				string str;
				std::vector<int> vec;
				bitset<32>wrd32;
				
				getline(file_in,str);
				 if( file_in.eof() ) break;
				 	
				extractIntegerWords(str,vec);

		        for(vector<int>::iterator itr = vec.begin(); itr != vec.end(); itr++)
		        {
						addr32 = line_address_data;
			            string d = addr32.to_string<char,std::string::traits_type,std::string::allocator_type>();
					    if(itr == vec.begin())
				    	{
				    		inst_name.erase(inst_name.size()-1);         //erase ":"
				    		data_label_map.insert(pair<string,string>(inst_name,d));
				    	}
					    d = nibble_hex(d,8);
					     d="0x"+d;
				    	file_out << d << "\t";
		        	wrd32 = *itr;
		        	string h = wrd32.to_string<char,std::string::traits_type,std::string::allocator_type>();
				    	h = nibble_hex(h,8);
				    	h="0x"+h;
				    	file_out << h<<endl;
				    	data_map.insert(pair<string,string>(d,h));    //d is address of each element
				    												//h is data
			    		line_address_data += 4;
		          }
		        }

			if(data_type == ".byte")
			{
				string str;
				std::vector<int> vec;
				bitset<8>wrd8;

				getline(file_in,str);
				 if( file_in.eof() ) break;
				 	
				extractIntegerWords(str,vec);

	                for(vector<int>::iterator itr = vec.begin(); itr != vec.end(); itr++)
	                {
						addr32  = line_address_data;
		                string d = addr32.to_string<char,std::string::traits_type,std::string::allocator_type>();
						if(itr == vec.begin())
					    {
					    		inst_name.erase(inst_name.size()-1);
					    		data_label_map.insert(pair<string,string>(inst_name,d));
					    }
						d = nibble_hex(d,8);
						d ="0x"+d;
				    	file_out << d << "\t";
	                	wrd8 = *itr;
	                	string h = wrd8.to_string<char,std::string::traits_type,std::string::allocator_type>();
				    	h = nibble_hex(h,2);
					    h="0x"+h;
				    	file_out << h<<endl;
				    	data_map.insert(pair<string,string>(d,h));   //d ia address and h data
			    		line_address_data += 1;
	                }
			}

		}

		//input from file_in  //****************************//

		else    //if(text!=0) or only text
		{	

			if( inst_name[inst_name.size()-1] == ':')
			{
			/*	label_name = inst_name;
				label_name.erase(label_name.size()-1);
				label_stage = 1;   //waiting for next instruction */
				continue;
			}
			bitset<32>bset32;
			string c,c1,c2,c3,c4;
			line_address_text += 4;
			bset32 = line_address_text;
		
			string b_l_a = bset32.to_string<char,std::string::traits_type,std::string::allocator_type>();
			string hex_address = nibble_hex(b_l_a,8);
			 hex_address = "0x" + hex_address;
			 file_out<<hex_address<<"\t";

			map<string, string>::iterator it = itype.find(inst_name);

			if(it==itype.end())
			{
				cout<<"command "<<inst_name<<" not found";
				return 0;
			}

			map<string, string>::iterator it2 = binary_f.find(inst_name);
			map<string, string>::iterator it_r1;
			map<string, string>::iterator it_r2;
			map<string, string>::iterator it_r3;
			map<string, string>::iterator it_rd;

			type = it->second;
			initial_bin = it2->second;

			string final_bin = initial_bin;   //changes are done in final_bin only;
			string  final_hex,rsd;
			int imm;

			///////////////////////////////////////////

			if(type == "R")
			{
				file_in >> rd >> rs1 >> rs2;
				 if( file_in.eof() ) break;
				 
				it_r1 = registers_num.find(rd);
				it_r2 = registers_num.find(rs1);
				it_r3 = registers_num.find(rs2);
				string a = it_r1->second;   //a
				string b = it_r2->second;   //b
				string c = it_r3->second;   //c

				final_bin.replace(20,5,a);
				final_bin.replace(12,5,b);
				final_bin.replace(7,5,c); 

				final_hex = nibble_hex(final_bin,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit
			   
			    final_hex = "0x" + final_hex;
				file_out << final_hex << endl;
			}
			//////////////////////////////////////////////////////

			else if(type == "I")
			{
				int imm;
				string a,b,s_bracket,rs;
				bitset<12> bset12;

				if(inst_name[0] == 'l'|| inst_name[0] == 'j')
				{

				file_in >> rd >> s_bracket;
				if( file_in.eof() ) break;
				it_rd = registers_num.find(rd);
						a = it_rd->second;   //b=rs2
					    //get imm and a i.e. rs1

					if(s_bracket[s_bracket.size()-1] == ')')	
					 {
						int flag = 0;
						string imm_str;
					    for (int i = 0; i < s_bracket.size(); ++i)
					    {
					    	if(s_bracket[i] == '(')
					    	{
					    		flag++;
					    		continue;
					    	}
					    	if(s_bracket[i] == ')')
					    		break;
					    	if(flag == 0)
					    	{
					    		imm_str+=s_bracket[i];
					    	}
					    	if(flag!=0)
					    	{
					    		rs+=s_bracket[i];
					    	}            	
					    }

					    it_r1 = registers_num.find(rs);
						 b = it_r1->second;   //b=rs1
					 
					 //change imm_str to int & store in imm
					    imm = atoi(imm_str.c_str());
					}
					else
					{

						map<string,string>::iterator it = data_label_map.find(s_bracket);
						if(it==data_label_map.end())
						{
							cout<<"word "<< s_bracket <<" not found";
						 return 0;
						}
						string label_address = it->second;
						string d_l_12 = label_address.substr(20,12);   //dest
					
						long long destination = binConv(d_l_12);
						destination     =   binaryToDecimal(destination);

						string auipc="00000000000000000000000000010111", two_t="00000000011111010000";
						auipc.replace(20,5,a);
						auipc.replace(0,20,two_t);
						string auipc_hex = nibble_hex(auipc,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit
			  		 	auipc_hex = "0x" + auipc_hex;
						file_out << auipc_hex  << endl;

						line_address_text += 4;
						bset32 = line_address_text;
						string b_l_a = bset32.to_string<char,std::string::traits_type,std::string::allocator_type>();
						string hex_address = nibble_hex(b_l_a,8);
						hex_address = "0x" + hex_address;
						file_out << hex_address<<"\t";

							///destination address - 2000
						int diff_adr = destination - 2000 ;
						imm = diff_adr ;
						b=a;
					}

				}
			    else
			    {
				    file_in >> rd >> rs1 >> imm;
				     if( file_in.eof() ) break;
				     
					it_r1 = registers_num.find(rd);
					it_r2 = registers_num.find(rs1);
					 a = it_r1->second;   //a
					 b = it_r2->second;   //b
				}

				if(inst_name[0] == 's')
				{
					bitset<6>bset6;
					bset6 = imm;
					string c = bset6.to_string<char,std::string::traits_type,std::string::allocator_type>();
					final_bin.replace(6,6,c);
				}
				else
				{
					bset12 = imm;
					string c = bset12.to_string<char,std::string::traits_type,std::string::allocator_type>();
					final_bin.replace(0,12,c);
				}

				final_bin.replace(20,5,a);
				final_bin.replace(12,5,b);
				final_hex = nibble_hex(final_bin,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit
			   
			    final_hex = "0x" + final_hex;
				file_out << final_hex << endl;
			}

			///////////////////////////////////

			 else if(type == "U")
			{
			    bitset<20> bset20;

			    string imm_str;
			    file_in >> rsd >> imm_str;
			     if( file_in.eof() ) break;
			     
				it_rd = registers_num.find(rsd);
				string a = it_rd->second;   //a

				final_bin.replace(20,5,a);

				imm = atoi(imm_str.c_str());

				bset20 = imm;
				string c = bset20.to_string<char,std::string::traits_type,std::string::allocator_type>();

			    final_bin.replace(0,20,c);

				final_hex = nibble_hex(final_bin,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit 
			    final_hex = "0x" + final_hex;
				file_out << final_hex << endl;
				//cout << final_hex << endl;
			    	
			}
			///////////////////////////////////////////

			 else if(type == "UJ")
			{
			    bitset<20> bset20;
				string label_str;

			    file_in >> rsd >> label_str;
			     if( file_in.eof() ) break;
			     
				it_rd = registers_num.find(rsd);
				string a = it_rd->second;   //a

				final_bin.replace(20,5,a);

				map<string,string>::iterator it=text_label_map.find(label_str);
				if(it==text_label_map.end())
				{
					cout<<"label "<< label_str <<" not found";
					return 0;
				}
				string label_address = it->second;

				string c_l_12 = b_l_a.substr(20,12);  //current
				string d_l_12 = label_address.substr(20,12);   //dest

				long long destination= binConv(d_l_12);  //atoi work properly for 10 digits max
				long long current    = binConv(c_l_12);
				int des    =     binaryToDecimal(destination);
				int  cur   =   	 binaryToDecimal(current);

					///destination address - current address

				int diff_adr = des - cur;
				imm = diff_adr;

				bset20 = imm;
				string c  = bset20.to_string<char,std::string::traits_type,std::string::allocator_type>();
				
				c.erase(c.size()-1);
				if( imm < 0 )
					c = "1" + c ;
				if ( imm > 0 )
					c = "0" + c ;	

				c1 = c.substr(0,1);
				c2 = c.substr(10,10);
				c3 = c.substr(9,1);
				c4 = c.substr(1,8);
				
					final_bin.replace(0,1 ,c1);	       
					final_bin.replace(1,10,c2);	       
					final_bin.replace(11,1,c3);	       
					final_bin.replace(12,8,c4);	       

				final_hex = nibble_hex(final_bin,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit 
			    final_hex = "0x" + final_hex;
				file_out << final_hex << endl;
			}

			/////////////////////////////////////////////////////////
			 else if(type=="SB")
			{
				bitset<12> bset12;

				string label_str;
				file_in>> rs1 >> rs2 >> label_str;
				 if( file_in.eof() ) break;
				 
				it_r1 = registers_num.find(rs1);
				it_r2 = registers_num.find(rs2);
				string a  = it_r1->second;   //a
				string b  = it_r2->second;   //b

				final_bin.replace(12,5,a);
				final_bin.replace(7,5,b);
		         
				map<string,string>::iterator it = text_label_map.find(label_str);
				if( it == text_label_map.end())
				{
					cout<<"label "<< label_str <<" not found";
					return 0;
				}
				string label_address = it->second;

				string c_l_12 = b_l_a.substr(20,12);  //current
				string d_l_12 = label_address.substr(20,12);   //dest

				long long destination = binConv(d_l_12);  //atoi work properly for 10 digits max
				long long current     = binConv(c_l_12);
				int des     =   binaryToDecimal(destination);
				int cur     =   binaryToDecimal(current);

					///destination address - current address
				int diff_adr = des - cur;
				imm = diff_adr;
				bset12 = imm;
				string c = bset12.to_string<char,std::string::traits_type,std::string::allocator_type>();
				
				c.erase(c.size()-1);
				if( imm < 0 )
					c = "1" + c ;
				if ( imm > 0 )
					c = "0" + c ;
				
				c1 = c.substr(0,1);
				c2 = c.substr(2,6);
				c3 = c.substr(8,4);
				c4 = c.substr(1,1);

				final_bin.replace(0,1,c1);
				final_bin.replace(1,6,c2);
				final_bin.replace(20,4,c3);
				final_bin.replace(24,1,c4);

				final_hex = nibble_hex(final_bin,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit
			    final_hex = "0x" + final_hex;
				file_out << final_hex << endl;        	
			}
			////////////////////////////////////////////////
			 else if(type == "S")
			{
				int imm;
				string s_bracket;
				string rs;
				bitset<12> bset12;

			    file_in >> rs2 >> s_bracket;
			     if( file_in.eof() ) break;
			     
				it_r2 = registers_num.find(rs2);
				string b = it_r2->second;   //b=rs2
			    //get imm and a i.e. rs1

				int flag = 0;
				string imm_str;
			    for (int i = 0; i < s_bracket.size(); ++i)
			    {
			    	if(s_bracket[i] == '(')
			    	{
			    		flag ++ ;
			    		continue;
			    	}
			    	if(s_bracket[i] == ')')
			    		break;
			    	if(flag == 0)
			    	{
			    		imm_str += s_bracket[i];
			    	}
			    	if(flag != 0)
			    	{
			    		rs += s_bracket[i];
			    	}            	
			    }

			    it_r1 = registers_num.find(rs);
				string a = it_r1->second;   //b=rs2
			 
			 //change imm_str to int & store in imm
			    imm = atoi(imm_str.c_str());

				bset12 = imm;
				string c = bset12.to_string<char,std::string::traits_type,std::string::allocator_type>();
				string c1,c2;
				c1 = c.substr(0,7);
				c2 = c.substr(7,5);
			     
				final_bin.replace(7,5,b);
				final_bin.replace(12,5,a);
				final_bin.replace(0,7,c1);
				final_bin.replace(20,5,c2);
		       

				final_hex = nibble_hex(final_bin,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit
			    final_hex = "0x" + final_hex;
				file_out << final_hex << endl;
			}

			/////////////////
		    else if(type=="E")
		    {
		    	
			    final_hex = nibble_hex(final_bin,8);  //string nibble_hex(string sin,int num_nibble) 8*4 for 32bit 
			    final_hex = "0x" + final_hex;
				file_out << final_hex << endl;
		    }


		       	else
			   cout<<"instrunction not found   ###error##";
	        		      
       	}
    } 
		
    file_in.close();

	return 0;
}


