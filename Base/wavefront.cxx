#include "wavefront.h"
#include <string.h>
#include <stdio.h>
#include <iostream>

const char* obj_database = "";	// ©w¸q mesh ªº¹w³]¥Ø¿ý

#pragma warning(disable:4786)	// microsoft stupid bug!!

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

mesh::mesh(const char* obj_file)
{
	matTotal = 0;		// mat[0] reserved for default meterial
	vTotal = tTotal = nTotal = fTotal = 0;
	
	Init(obj_file);
}

mesh::mesh()
{
	matTotal = 0;			
	vTotal = tTotal = nTotal = fTotal = 0;
}

mesh::~mesh()
{
}

void mesh::LoadMesh(string obj_file)
{
	char	token[100], buf[100], v[5][100];	// v[5] ªí¥Ü¤@­Ó polygon ¥i¥H¦³ 5­Ó vertex
	float	vec[3];
	int i = 0;
	int	n_vertex, n_texture, n_normal;
	int	cur_tex = 0;				// state variable: ¥Ø«e©Ò¨Ï¥Îªº material

	scene = fopen(obj_file.c_str(),"r");
	s_file = obj_file;

	if (!scene) 
	{
		cout<< string("Can not open object File \"") << obj_file << "\" !" << endl;
		return;
	}

	cout<<endl<<obj_file<<endl;
		
	while(!feof(scene))
	{
		token[0] = (char)NULL;
		fscanf(scene,"%s", token);		// Åª token

		if (!strcmp(token,"g"))
		{
			fscanf(scene,"%s",buf);
		}

		else if (!strcmp(token,"mtllib"))
		{
  			fscanf(scene,"%s", mat_file);
			LoadTex(string(obj_database) + string(mat_file));
		}

		else if (!strcmp(token,"usemtl"))
		{
			fscanf(scene,"%s",buf);
			cur_tex = matMap[s_file+string("_")+string(buf)];
		}

		else if (!strcmp(token,"v"))
		{
			fscanf(scene,"%f %f %f",&vec[0],&vec[1],&vec[2]);
			vList.push_back(Vec3(vec));
		}

		else if (!strcmp(token,"vn"))
		{
			fscanf(scene,"%f %f %f",&vec[0],&vec[1],&vec[2]);
			nList.push_back(Vec3(vec));
		}
		else if (!strcmp(token,"vt"))
		{
			fscanf(scene,"%f %f",&vec[0],&vec[1]);
			tList.push_back(Vec3(vec));
		}

		else if (!strcmp(token,"f"))
		{
			i = 0;
			for (i=0;i<3;i++)		// face ¹w³]¬° 3¡A°²³]¤@­Ó polygon ³£¥u¦³ 3 ­Ó vertex
			{
				fscanf(scene,"%s",v[i]);
				//printf("[%s]",v[i]);
			}
			//printf("\n");
		  	i = 0;
			Vertex	tmp_vertex[3];		// for faceList structure

			for (i=0;i<3;i++)		// for each vertex of this face
			{
				char str[20], ch;
				int base,offset;
				base = offset = 0;

				// calculate vertex-list index
				while( (ch=v[i][base+offset]) != '/' && (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_vertex = atoi(str);
				base += (ch == '\0')? offset : offset+1;
				offset = 0;

				// calculate texture-list index
				while( (ch=v[i][base+offset]) != '/' && (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_texture = atoi(str);	// case: xxx//zzz¡Atexture ³]¬° 0 (tList ±q 1 ¶}©l)
				base += (ch == '\0')? offset : offset+1;
				offset = 0;

				// calculate normal-list index
				while( (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_normal = atoi(str);	// case: xxx/yyy¡Anormal ³]¬° 0 (nList ±q 1 ¶}©l)

				tmp_vertex[i].v = n_vertex;
				tmp_vertex[i].t = n_texture;
				tmp_vertex[i].n = n_normal;
				tmp_vertex[i].m = cur_tex;
			}

			faceList.push_back(FACE(tmp_vertex[0],tmp_vertex[1],tmp_vertex[2]));
		}

		else if (!strcmp(token,"#"))	  // µù¸Ñ
			fgets(buf,100,scene);

//		printf("[%s]\n",token);
	}

	if (scene) fclose(scene);

	vTotal = vList.size();
	nTotal = nList.size();
	tTotal = tList.size();
	fTotal = faceList.size();
	printf("vetex: %d, normal: %d, texture: %d, triangles: %d\n",vTotal, nTotal, tTotal, fTotal);
}

void mesh::LoadTex(string tex_file)
{
	char	token[100], buf[100], v1[100], v2[100], v3[100];
	float	x,y,z,r,g,b;

	int	n_vertex, n_texture, n_normal;		// temp ªº current data
	int	cur_tex;				// Àx¦s±µµÛ¤U¥hªº polygon ¬O¥Î­þ­Ó texture

	texture = fopen(tex_file.c_str(),"r");
	t_file = tex_file;

	if (!texture) 
	{
		cout << "Can't open material file \"" << tex_file << "\"!" << endl;
		return;
	}

	cout<<tex_file<<endl;

	int cur_mat;

	while(!feof(texture))
	{
		token[0] = (char)NULL;
		fscanf(texture,"%s", token);		// Åª token

		if (!strcmp(token,"newmtl"))
		{
			fscanf(texture,"%s",buf);
			cur_mat = matTotal++;					// ±q mat[1] ¶}©l¡Amat[0] ªÅ¤U¨Óµ¹ default material ¥Î
			matMap[s_file+string("_")+string(buf)] = cur_mat; 	// matMap["material_name"] = material_id;
		}

		else if (!strcmp(token,"Ka"))
		{
			fscanf(texture,"%f %f %f",&r,&g,&b);
			mat[cur_mat].Ka[0] = r;
			mat[cur_mat].Ka[1] = g;
			mat[cur_mat].Ka[2] = b;
			mat[cur_mat].Ka[3] = 1;
		}

		else if (!strcmp(token,"Kd"))
		{
			fscanf(texture,"%f %f %f",&r,&g,&b);
			mat[cur_mat].Kd[0] = r;
			mat[cur_mat].Kd[1] = g;
			mat[cur_mat].Kd[2] = b;
			mat[cur_mat].Kd[3] = 1;
		}

		else if (!strcmp(token,"Ks"))
		{
			fscanf(texture,"%f %f %f",&r,&g,&b);
			mat[cur_mat].Ks[0] = r;
			mat[cur_mat].Ks[1] = g;
			mat[cur_mat].Ks[2] = b;
			mat[cur_mat].Ks[3] = 1;
		}

		else if (!strcmp(token,"Ns"))
		{
			fscanf(texture,"%f",&r);
			mat[cur_mat].Ns = r;
		}

		else if (!strcmp(token,"#"))	  // µù¸Ñ
			fgets(buf,100,texture);

//		printf("[%s]\n",token);
	}

	printf("total material:%d\n",matMap.size());

	if (texture) fclose(texture);
}

void mesh::Init(const char* obj_file)
{
	float default_value[3] = {1,1,1};
      
	vList.push_back(Vec3(default_value));	// ¦]¬° *.obj ªº index ¬O±q 1 ¶}©l
	nList.push_back(Vec3(default_value));	// ©Ò¥H­n¥ý push ¤@­Ó default value ¨ì vList[0],nList[0],tList[0]
	tList.push_back(Vec3(default_value));

	// ©w¸q default meterial: mat[0]
	mat[0].Ka[0] = 0.0; mat[0].Ka[1] = 0.0; mat[0].Ka[2] = 0.0; mat[0].Ka[3] = 1.0; 
	mat[0].Kd[0] = 1.0; mat[0].Kd[1] = 1.0; mat[0].Kd[2] = 1.0; mat[0].Kd[3] = 1.0; 
	mat[0].Ks[0] = 0.8; mat[0].Ks[1] = 0.8; mat[0].Ks[2] = 0.8; mat[0].Ks[3] = 1.0;
	mat[0].Ns = 32;
	matTotal++;
	LoadMesh(string(obj_file));		
		
	this->listid = glGenLists(1);
	printf("Listid is :  %i\n",this->listid);
	glNewList(this->listid,GL_COMPILE);
	glBegin(GL_TRIANGLES);
		for (int i=0;i<this->fTotal;i++)
{
   for (int j=0;j<3;j++)
   {
      //this->mat[this->faceList[i][j].m].Ka
     glTexCoord2f(this->vList[this->faceList[i][j].v].ptr[1],this->vList[this->faceList[i][j].v].ptr[2]);
      glNormal3f(this->nList[this->faceList[i][j].n].ptr[0],this->nList[this->faceList[i][j].n].ptr[1],this->nList[this->faceList[i][j].n].ptr[2]);
      
      glVertex3f(this->vList[this->faceList[i][j].v].ptr[0],this->vList[this->faceList[i][j].v].ptr[1],this->vList[this->faceList[i][j].v].ptr[2]);
     
     
   }
   
}
	glEnd();
	glEndList();
		// Åª¤J .obj ÀÉ (¥i³B²z Material)
	
}
void mesh::Render(float x,float y,float z,float rx,float ry,float rz)
{
		
		
		glPushMatrix();
		glTranslatef(x,y,z);
		glRotatef(rx,1,0,0);
		glRotatef(ry,0,1,0);
		glRotatef(rz,0,0,1);
		/*
		glBegin(GL_TRIANGLES);
		for (int i=0;i<this->fTotal;i++)
{
   for (int j=0;j<3;j++)
   {
      //this->mat[this->faceList[i][j].m].Ka
     glTexCoord3f(this->vList[this->faceList[i][j].v].ptr[0],this->vList[this->faceList[i][j].v].ptr[1],this->vList[this->faceList[i][j].v].ptr[2]);
      glNormal3f(this->nList[this->faceList[i][j].n].ptr[0],this->nList[this->faceList[i][j].n].ptr[1],this->nList[this->faceList[i][j].n].ptr[2]);
      
      glVertex3f(this->vList[this->faceList[i][j].v].ptr[0],this->vList[this->faceList[i][j].v].ptr[1],this->vList[this->faceList[i][j].v].ptr[2]);
     
     
   }
}
	glEnd();*/
		glCallList(this->listid);
		glPopMatrix();
}
