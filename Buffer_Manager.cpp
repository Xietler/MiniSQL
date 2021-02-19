#include "Buffer_Manager.h"

FILEINFO* Buffer_Manager::get_file_info(string DB_Name, string Table_Name){
	FILEINFO* fip;
	for(fip = fileHandler; fip!=NULL; fip=fip->next){
		if(fip->fileName == Table_Name)
			return fip;
	}
	fip = findFile(DB_Name, Table_Name);
	fip->fileName = Table_Name;
	fip->firstBlock = NULL;

	return fip;
}

BLOCKINFO* Buffer_Manager::get_file_block(string DB_Name, string Table_Name, int blockNum){
	FILEINFO* fip;
	BLOCKINFO* bip;
	FILE *fp;
	for(fip = fileHandler; fip!=NULL; fip=fip->next){
		if(fip->fileName == Table_Name){
			bip = fip->firstBlock;
			for(; bip!=NULL; bip=bip->fnext){
				if(bip->blockNum == blockNum)
					return bip;
			}
			break;
		}
	}
	if(fip == NULL){
		fip = findFile(DB_Name, Table_Name);
	}
	bip = findBlock(DB_Name, Table_Name);
	
	memset(path, 0, 100);
	strcat(path,"./");
	strcat(path,DB_Name.c_str());
	strcat(path,"/");
	strcat(path,Table_Name.c_str());
	strcat(path,".data");
	
	
	if(!(fp = fopen(path, "rb+")))
		fp = fopen(path,"wb+");
	fseek(fp,0,SEEK_END);
	int ps = ftell(fp);
	//if(ps >= (blockNum+1)*BLOCK_LEN){
		bip->byteNum = BLOCK_LEN;
	//}
	//else{
	//	bip->byteNum = ps-(blockNum*BLOCK_LEN);
	//}
	fseek(fp,blockNum*BLOCK_LEN,SEEK_SET);
	fread(bip->cBlock,BLOCK_LEN,1,fp);
	bip->blockNum = blockNum;
	bip->dirtyBit = 0;
	bip->file = fip;
	bip->fnext = fip->firstBlock;
	fip->firstBlock = bip;
	bip->iTime = Time;
	bip->lock = 0;
	Time++;
	fclose(fp);
	return bip;
}



BLOCKINFO* Buffer_Manager::inc_file_block(string DB_Name, string Table_Name, int blockNum){
	BLOCKINFO* bip;
	FILEINFO* fip;
	fip = get_file_info(DB_Name, Table_Name);
	bip = findBlock(DB_Name, Table_Name);
	bip->fnext = fip->firstBlock;
	fip->firstBlock = bip;
	bip->blockNum = blockNum;
	bip->byteNum = 0;
	bip->dirtyBit = 0;
	bip->file = fip;
	bip->iTime = Time;
	bip->lock = 0;
	Time++;
	return bip;
}

void Buffer_Manager::close_file(string DB_Name, FILEINFO* fip){
	BLOCKINFO* bip;
	BLOCKINFO* tip;
	fileSize--;
	for(bip=blockHandler; bip!=NULL;){
		tip = bip;
		bip = bip->bnext;
		if(blockHandler->file == fip){
			blockHandler = bip;
			if(tip->dirtyBit == 1){
				memset(path, 0, 100);
				strcat(path,"./");
				strcat(path,DB_Name.c_str());
				strcat(path,"/");
				strcat(path,fip->fileName.c_str());
				strcat(path,".data");

				FILE *fp;
				if(!(fp = fopen(path,"rb+")))
					fp = fopen(path,"wb+");
				fseek(fp,BLOCK_LEN*tip->blockNum,SEEK_SET);
				fwrite(tip->cBlock,tip->byteNum,1,fp);
				fclose(fp);
			}
			delete tip->cBlock;
			delete tip;
			blockSize--;
		}
		else if(bip == NULL){
			break;
		}
		else if(bip->file == fip){
			tip->bnext = bip->bnext;
			if(bip->dirtyBit == 1){
				memset(path, 0, 100);
				strcat(path,"./");
				strcat(path,DB_Name.c_str());
				strcat(path,"/");
				strcat(path,fip->fileName.c_str());
				strcat(path,".data");

				FILE *fp;
				if(!(fp = fopen(path,"rb+")))
					fp = fopen(path,"wb+");
				fseek(fp,BLOCK_LEN*bip->blockNum,SEEK_SET);
				fwrite(bip->cBlock,bip->byteNum,1,fp);
				fclose(fp);
			}
			delete[] bip->cBlock;
			delete bip;
			bip=tip;
			blockSize--;
		}
	}
	fip = NULL;
}

BLOCKINFO* Buffer_Manager::findBlock(string DB_Name, string Table_Name){
	BLOCKINFO *bip,*tip;
	FILEINFO* fip;
	if(blockSize == MAX_BLOCK){
		unsigned int minTime=0x7FFF;
		for(bip=blockHandler; bip!=NULL; bip=bip->bnext){
			if(bip->iTime < minTime){
				minTime = bip->iTime;
				tip = bip;
				fip = bip->file;
			}
		}
		for(bip=fip->firstBlock; bip!=NULL; bip=bip->fnext){
			if(fip->firstBlock == tip){
				fip->firstBlock = bip->fnext;
				break;
			}
			else if(bip->fnext == tip){
				bip->fnext = tip->fnext;
				break;
			}
		}
		if(tip->dirtyBit == 1){
			memset(path, 0, 100);
			strcat(path,"./");
			strcat(path,DB_Name.c_str());
			strcat(path,"/");
			strcat(path,tip->file->fileName.c_str());
			strcat(path,".data");

			FILE *fp;
			//if(!(fp = fopen(path,"rb+")))
			//	fp = fopen(path,"wb+");
			fp = fopen(path, "rb+");
			fseek(fp,BLOCK_LEN*tip->blockNum,SEEK_SET);
			fwrite(tip->cBlock,tip->byteNum,1,fp);
			fclose(fp);
		}
		return tip;
	}
	else{
		blockSize++;

		bip = new BLOCKINFO;
		bip->cBlock = new BYTE[BLOCK_LEN];
		bip->bnext = blockHandler;
		blockHandler = bip;
		return bip;
	}
}

FILEINFO* Buffer_Manager::findFile(string DB_Name, string Table_Name){
	FILEINFO *fip,*tip;
	if(fileSize == MAX_FILE_ACTIVE){
		fip = fileHandler;
		fileHandler = fileHandler->next;
		close_file(DB_Name, fip);
	}
	fileSize++;
	fip = new FILEINFO;
	fip->firstBlock = NULL; 
	fip->next = NULL;
	fip->fileName = Table_Name; 
	if(fileHandler==NULL){
		fileHandler = fip;
		return fip;
	}
	for(tip=fileHandler; tip->next!=NULL; tip=tip->next) ;
	tip->next = fip;
	return fip;
}

void Buffer_Manager::close_database(string DB_Name){
	FILEINFO* fip;
	fip = fileHandler;
	while(fip!=NULL){
		fileHandler = fileHandler->next;
		close_file(DB_Name, fip);
		fip = fileHandler;
	}
}

void Buffer_Manager::delete_file(string DB_Name, string Table_Name) {
	FILEINFO* fip;
	BLOCKINFO* bip;
	BLOCKINFO* tip;
	for(fip = fileHandler; fip!=NULL; fip=fip->next){
		if(fip->fileName == Table_Name){
			break;
		}
	}
	fileSize--;
	for(bip=blockHandler; bip!=NULL;){
		tip = bip;
		bip = bip->bnext;
		if(blockHandler->file == fip){
			blockHandler = bip;
			delete tip->cBlock;
			delete tip;
			blockSize--;
		}
		else if(bip == NULL){
			break;
		}
		else if(bip->file == fip){
			tip->bnext = bip->bnext;
			delete[] bip->cBlock;
			delete bip;
			bip=tip;
			blockSize--;
		}
	}
	fip = NULL;
}

Buffer_Manager::Buffer_Manager(){
	fileHandler = NULL;
	blockHandler = NULL;
	blockSize = 0;
	fileSize = 0;
	Time = 0;
}
