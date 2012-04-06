//Chris.cpp

#include "did.h"
#include <fstream>
#include <iostream>

void db();

int main(){
	ifstream pic;
	pic.open("andy.jpg", ios::in | ios::binary | ios::ate);
	
	ifstream::pos_type size;
	char *img;
	
	if(pic.is_open()){
		size = pic.tellg();
		cout << "size: " << (long)size << endl;
		img = new char[size];
		pic.seekg(0, ios::beg);
		pic.read(img, size);
		pic.close();
	} else{
		return -1;
	}
	
	Message *m = new Message();
	m->setCmd("this is a command!");
	m->setImg(img, (long)size);
	
	delete[] img;
	
	unsigned char *raw = m->serialize();
	// to lower layers
	
	// from lower layers
	Message *moreOfTheSame = Message::unserialize(raw);
	cout << m->getCmd() << endl;
	
	long newSize = m->getContentSize();
	char *newImg = (char *) calloc(newSize, sizeof(char));
	memcpy(newImg, m->getContent(), newSize);
	
	//FILE *tmpFile;
	//tmpFile = tmpFile();
	
	//fwrite(newImg, sizeof(char), newSize, tmpFile);
	
	if(connectToDB()){
		if(addPhotoPublic("2", newImg, newSize)){
			cout << "added" << endl;
		}
		unsigned long fsize = 0;
		char *newImg = getPhotoPublic("1", &fsize);
		ofstream newPic;
		newPic.open("new.jpg", ios::out|ios::binary);
	 	if(newPic.is_open()){
	 		newPic.write(newImg, fsize);
	 		newPic.close();
	 	}
	}
	
	disconnectFromDB();
	//fclose(tmpFile);
	
	return 0;
}

void db(){
	cout << "Connecting to database" << endl;
	bool connected = connectToDB();
	if (connected){
		cout << "Connected!" << endl;
		//int rws = getRowCountPublic();
		//cout << createMissing("Bob", "Smith", "Worcester, MA") << endl;
		//cout << locationsWithMissing() << endl;
		//if(createMissingAdmin("000000008", "Haverhill, MA", "John", "Dough")){
		//	cout << "it worked!" << endl;
		//}
		//if(createMissingAdmin("000000009", "Acton, MA", NULL, NULL)){
		//	cout << "it also worked!" << endl;
		//}
		//if(positiveID("000000009", "Bill", "Jones")){
		//	cout << "\\m/" << endl;
		//}
		//cout << locationsWithMissing() << endl;
		//if(removeFromAdmin("000000002")){
		//	cout << "it wurk'd!" << endl;
		//}
		//if(removeFromPublic("5")){
		//	cout << "it also wurk'd!" << endl;
		//}
// 		if(newUser("boss", "abc123")){
// 			cout << "yhusssss" << endl;
// 		}
// 		if(changePassword("boss", "abc123", "123abc")){
// 			cout << "yay!" << endl;
// 		}
	} else{
		cout << "Connection error!" << endl;
	}
	disconnectFromDB();
}
