#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "functions.h"

/* ESTAS FUNCIONES NO HAY QUE TOCARLAS */
/* SI QUEREIS PONER FUNCIONES NUEVAS   */
/* HACERLO AL FINAL DEL FICHERO        */

//NO TOCAR
void Crypt(int cry[], int key, int len){
	int i;
    for(i=0;i<len;++i){
        cry[i] = cry[i]  ^ key;
    }
}

//NO TOCAR
void Decrypt(int key, int partNum){
	int fd, len,i;
	char cryptfile[15]="";
	char decryptfile[15]="";
	int *cry;
	FILE *fdec;

	sprintf(cryptfile,"cryptpart%d",partNum+1);
	sprintf(decryptfile,"decryptpart%d",partNum+1);
	fd=open(cryptfile,O_RDONLY);
	if (fd){
		//leemos longitud de la parte
		len=GetLength(cryptfile);
		//printf("decryptpart:%d Len:%d\n",partNum,len);
		cry=(int*)malloc(len*sizeof(int));
		//Cargamos encriptado
		read(fd,cry,len*sizeof(int));
		//Cerramos fichero
		close(fd);
		cry=cry+1;
		//Desencriptamos
		Crypt(cry,key,len);
		//Guardamos encriptado
		
		fdec=fopen(decryptfile,"w");
		if (fdec){
			for(i=0;i<len-1;i++) 
				fprintf(fdec, "%c",(char)cry[i]);
			fclose(fdec);
			}
		else{
			printf("Fallo al desencriptar %s\n",strerror(errno));
			exit(1);
			}
	}
	else {
		printf("Fallo al desencriptar\n");
		exit(1);
	}
}			


//NO TOCAR
int GetLength(char *filename){
	int fdl,len=0;
	
	fdl=open(filename,O_RDWR|O_CREAT,S_IRWXU);
	if (fdl!=-1){
		read(fdl,&len,sizeof(int)); 
		close(fdl);
		return len;
		}
	else {
		printf("Fallo GetLength: %s\n",strerror(errno));
		exit(1);
		}		
}

//NO TOCAR
void SetLength(int len){
	int fdl;
	
	fdl=open(".cryptl",O_RDWR|O_CREAT,S_IRWXU);
	if (fdl!=-1){
		write(fdl,&len,sizeof(int)); 
		close(fdl);
		}
	else {
		printf("Fallo SetLength: %s\n",strerror(errno));
		
		exit(1);
		}		
}

//NO TOCAR
void LoadCrypted(char *cryptfile, int *cryptint, int len){
	int fd;
	fd=open(cryptfile,O_RDONLY);
	read(fd,cryptint,len*sizeof(int));
	close(fd);
}

//NO TOCAR
void CalculatePartSizes(int *partsizes, int nparts, int len){
	int i,resto=0;
	int *tmpsizes=(int*)malloc((nparts+1)*sizeof(int));
	
	resto=len%nparts;
	//printf("nparts:%d len:%d resto:%d\n",nparts,len,resto);
	
	for(i=0; i<nparts; i++)
		tmpsizes[i]=(int)(len/(nparts));

	if (resto!=0){
		for(i=0;i<resto;i++)
			tmpsizes[i]+=1;
	}
	/*
	for(i=0;i<nparts;i++)
		printf("tmpsizes[%d]=%d\n",i,tmpsizes[i]);
	*/
	
	partsizes[0]=tmpsizes[0];
	for(i=1;i<nparts;i++){
		partsizes[i]=partsizes[i-1]+tmpsizes[i];
		}

	/*
	for(i=0;i<nparts;i++)
		printf("partsizes[%d]=%d\n",i,partsizes[i]);
	*/
	
	free(tmpsizes);
}

//NO TOCAR
void SavePart(int partnum, int *partsizes, char *sufix, int *intvalues){
	int ix=partnum;
	int partsize;
	int fd,i,fromix=0,toix=0,*part;
	char partname[100]="";

	//printf("Saving part[%d] last:[%d] ",ix,partsizes[partnum]);
	
	if (partnum==0){ 
		fromix=0;
    	toix=partsizes[0]-1;
    	partsize=partsizes[0]+1;
    	}
    else {
    	fromix=partsizes[partnum-1];
    	toix=partsizes[partnum];
    	partsize=toix-fromix+1;
    	}
	//printf("From:%d To:%d Len:%d\n",fromix,toix,partsize);

	sprintf(partname,"%s%d",sufix,partnum+1);
	fd=open(partname,O_WRONLY|O_CREAT,S_IRUSR | S_IWUSR);
	if (fd!=-1){
		write(fd,&partsize,sizeof(int));
		part=&(intvalues[fromix]);
		write(fd,part,(partsize)*sizeof(int));
		close(fd);
		}
	else {
		printf("Fallo SavePart part%d\n",partnum);
		exit(1);
		}
}

//NO TOCAR
void Partir(int nparts){
	int *partsizes,*cryptints,len,i;
	
    partsizes=(int*)malloc((nparts)*sizeof(int));
	len=GetLength(".cryptl");
	CalculatePartSizes(partsizes,nparts,len);
	cryptints=(int*)malloc(len*sizeof(int));
	LoadCrypted(".crypt",cryptints,len);
	for(i=0;i<nparts;i++)
		SavePart(i,partsizes,"cryptpart",cryptints);
}

/*------------------------------------------------- */
/* A PARTIR DE AQUI PONER LAS FUNCIONES QUE QUERAIS */




