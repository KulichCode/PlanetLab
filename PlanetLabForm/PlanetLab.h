#pragma once

// include the basic windows header files and the Direct3D header file
//#include <stdafx.h>
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <stdio.h>

// include the Direct3D Library files
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment(lib, "user32.lib")

// global declarations
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;

D3DXVECTOR3  camLookAt, camPos;

float width, height;

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)

struct _VERTEX
{
	D3DXVECTOR3 pos;     // vertex position
	D3DXVECTOR3 norm;    // vertex normal
	float tu;            // texture coordinates
	float tv;
} VERTEX, *LPVERTEX;

#define FVF_VERTEX    D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1

LPD3DXMESH CreateMappedSphere(LPDIRECT3DDEVICE9 pDev, float fRad, UINT slices, UINT stacks)
{
	// create the sphere
	LPD3DXMESH mesh;
	if (FAILED(D3DXCreateSphere(pDev, fRad, slices, stacks, &mesh, NULL)))
		return NULL;

	// create a copy of the mesh with texture coordinates,
	// since the D3DX function doesn't include them
	LPD3DXMESH texMesh;
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, FVF_VERTEX, pDev, &texMesh)))
		// failed, return un-textured mesh
		return mesh;

	// finished with the original mesh, release it
	mesh->Release();

	// lock the vertex buffer
	texMesh->LockVertexBuffer(0, (LPVOID *)&LPVERTEX);

	// get vertex count
	int numVerts = texMesh->GetNumVertices();

	// loop through the vertices
	for (int i = 0; i < numVerts; i++) {

		// calculate texture coordinates
		LPVERTEX->tu = asinf(LPVERTEX->norm.x) / D3DX_PI + 0.5f;
		LPVERTEX->tv = asinf(LPVERTEX->norm.y) / D3DX_PI + 0.5f;

		// go to next vertex
		LPVERTEX++;
	}

	// unlock the vertex buffer
	texMesh->UnlockVertexBuffer();

	// return pointer to caller
	return texMesh;
}

struct Planet
{
	float distanceFromSun;
	float speed;
	float speedIncrement;
	D3DMATERIAL9 material;
	LPD3DXMESH mesh;
	LPDIRECT3DVOLUMETEXTURE9 texture;
	void setup(float _radius, float _distanceFromSun, float _speedIncrement, LPCSTR _texture, float _brightness);
}Planets[10];

void Planet::setup(float _radius, float _distanceFromSun, float _speedIncrement, LPCSTR _texture, float _brightness)
{
	distanceFromSun = _distanceFromSun;
	speed = 0.0f;
	speedIncrement = _speedIncrement;
	mesh = CreateMappedSphere(d3ddev, _radius, 64, 64);
	D3DXCreateVolumeTextureFromFileA(d3ddev, _texture, &texture);
	material.Emissive = D3DXCOLOR(_brightness, _brightness, _brightness, 1.0f);
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

void CameraUpDown(float angle)
{
	float distance = sqrt(pow((camPos.x - camLookAt.x), 2) + pow((camPos.y - camLookAt.y), 2) + pow((camPos.z - camLookAt.z), 2));
	float phi = atan2(camPos.z, camPos.x);
	float theta = acos(camPos.y / distance);
	theta += D3DXToRadian(angle);
	if (theta < 0.087268)
	{
		theta = 0.087268;
	}
	else if (theta > 3.14159)
	{
		theta = 3.141;
	}
	camPos.x = distance * sin(theta) * cos(phi);
	camPos.z = distance * sin(theta) * sin(phi);
	camPos.y = distance * cos(theta);
}

void CameraLeftRight(float angle)
{
	float distance = sqrt(pow((camPos.x - camLookAt.x), 2) + pow((camPos.y - camLookAt.y), 2) + pow((camPos.z - camLookAt.z), 2));
	float phi = atan2(camPos.z, camPos.x);
	float theta = acos(camPos.y / distance);

	phi += D3DXToRadian(angle);

	camPos.x = distance * sin(theta) * cos(phi);
	camPos.z = distance * sin(theta) * sin(phi);
}

void CameraZoom(float units)
{
	float distanceOld = sqrt(pow((camPos.x - camLookAt.x), 2) + pow((camPos.y - camLookAt.y), 2) + pow((camPos.z - camLookAt.z), 2));
	float distanceNew = distanceOld + units;
	if (distanceNew < 1)
	{
		distanceNew = 1;
	}
	float k = distanceNew / distanceOld;
	camPos *= k;
}

void Hightlight(int index)
{
	Planets[index + 1].material.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

void Shade(int index)
{
	Planets[index + 1].material.Emissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.1f);
}

bool swapped = false;
void SwapTexture()
{
	if (!swapped)
	{
		Planets[9].texture->Release();
		D3DXCreateVolumeTextureFromFileA(d3ddev, "Textures/Machy.jpg", &Planets[9].texture);
		swapped = true;
	}
	else
	{
		Planets[9].texture->Release();
		D3DXCreateVolumeTextureFromFileA(d3ddev, "Textures/Background2.jpg", &Planets[9].texture);
		swapped = false;
	}
}

// function prototypes
void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void);

bool rotate = true;

namespace PlanetLabForm
{
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	std::string plname[] = { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune" };
	std::string plarea[] = { "0.147", "0.902", "1", "0.284", "120", "83.7", "15.8", "14.9" };
	std::string plradius[] = { "2439.7", "6051.85", "6378", "3396.2", "71 492", "60 268", "25 559", "24 764" };
	std::string plmass[] = { "0.055", "0.815", "1", "0.531", "120.5", "95.162", "14.536", "17.1" };
	std::string pldens[] = { "5.427", "5.204", "5.515", "3.933", "1.326", "0.6873", "1.270", "1.638" };
	std::string plsundist[] = { "0.307", "0.718", "1", "1.381", "4.951", "9.020", "18.286", "29.766" };
	std::string platmo[] = { "0", "9 321", "100", "0.9", "110", "140", "120", ">>100" };
	std::string plshi[] = { "54/0/46", "80/0/20", "75/1/24", "90/<1/10", "2/98/0", "2/97/1", "14/89/5", "14/81/5" };

	/// <summary>
	/// Summary for PlanetLab
	/// </summary>
	public ref class PlanetLab : public System::Windows::Forms::Form
	{
	public: int currentplanet = 0;


	public:
		PlanetLab(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			width = window->Width;
			height = window->Height;
			initD3D((HWND)window->Handle.ToPointer());
			RefreshValues();
			Hightlight(currentplanet);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~PlanetLab()
		{
			if (components)
			{
				delete components;
			}
			cleanD3D();
		}

	private: void RefreshValues()
	{
		planetName->Text = gcnew System::String(plname[currentplanet].c_str());
		planetArea->Text = gcnew System::String(plarea[currentplanet].c_str()) + " Earth";
		planetRadius->Text = gcnew System::String(plradius[currentplanet].c_str()) + " km";;
		planetMass->Text = gcnew System::String(plmass[currentplanet].c_str()) + " Earth";;
		planetDensity->Text = gcnew System::String(pldens[currentplanet].c_str()) + " g/cm3";;
		planetSunDist->Text = gcnew System::String(plsundist[currentplanet].c_str()) + " AU";;
		planetAtmo->Text = gcnew System::String(platmo[currentplanet].c_str()) + " kPa";;
		planetSHI->Text = gcnew System::String(plshi[currentplanet].c_str());
	}
	public: System::Windows::Forms::Panel^  window;
	private: System::Windows::Forms::Button^  btExit;
	private: System::Windows::Forms::Button^  btNeptune;
	private: System::Windows::Forms::Button^  btUranus;
	private: System::Windows::Forms::Button^  btSaturn;
	private: System::Windows::Forms::Button^  btJupiter;
	private: System::Windows::Forms::Button^  btMars;
	private: System::Windows::Forms::Button^  btEarth;
	private: System::Windows::Forms::Button^  btVenus;
	private: System::Windows::Forms::Button^  btMercury;
	private: System::Windows::Forms::Label^  planetSHI;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  planetAtmo;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  planetSunDist;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  planetDensity;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  planetMass;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  planetRadius;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  planetArea;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  planetName;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Panel^  btPanel;

	private:


	public:
	protected:


	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PlanetLab::typeid));
			this->window = (gcnew System::Windows::Forms::Panel());
			this->btExit = (gcnew System::Windows::Forms::Button());
			this->btMercury = (gcnew System::Windows::Forms::Button());
			this->btNeptune = (gcnew System::Windows::Forms::Button());
			this->btUranus = (gcnew System::Windows::Forms::Button());
			this->btSaturn = (gcnew System::Windows::Forms::Button());
			this->btJupiter = (gcnew System::Windows::Forms::Button());
			this->btMars = (gcnew System::Windows::Forms::Button());
			this->btEarth = (gcnew System::Windows::Forms::Button());
			this->btVenus = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->planetName = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->planetArea = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->planetRadius = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->planetMass = (gcnew System::Windows::Forms::Label());
			this->planetSHI = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->planetDensity = (gcnew System::Windows::Forms::Label());
			this->planetAtmo = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->planetSunDist = (gcnew System::Windows::Forms::Label());
			this->btPanel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// window
			// 
			this->window->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->window->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->window->BackColor = System::Drawing::SystemColors::Control;
			this->window->Location = System::Drawing::Point(0, 1);
			this->window->Name = L"window";
			this->window->Size = System::Drawing::Size(1088, 693);
			this->window->TabIndex = 0;
			// 
			// btExit
			// 
			this->btExit->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btExit->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btExit->Enabled = false;
			this->btExit->Location = System::Drawing::Point(1094, 626);
			this->btExit->Name = L"btExit";
			this->btExit->Size = System::Drawing::Size(158, 43);
			this->btExit->TabIndex = 50;
			this->btExit->TabStop = false;
			this->btExit->Text = L"Exit";
			this->btExit->UseVisualStyleBackColor = true;
			this->btExit->Click += gcnew System::EventHandler(this, &PlanetLab::btExit_Click);
			// 
			// btMercury
			// 
			this->btMercury->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btMercury->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btMercury->Enabled = false;
			this->btMercury->Location = System::Drawing::Point(1094, 394);
			this->btMercury->Name = L"btMercury";
			this->btMercury->Size = System::Drawing::Size(158, 23);
			this->btMercury->TabIndex = 42;
			this->btMercury->TabStop = false;
			this->btMercury->Text = L"Mercury";
			this->btMercury->UseVisualStyleBackColor = true;
			this->btMercury->Click += gcnew System::EventHandler(this, &PlanetLab::btMercury_Click);
			// 
			// btNeptune
			// 
			this->btNeptune->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btNeptune->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btNeptune->Enabled = false;
			this->btNeptune->ForeColor = System::Drawing::SystemColors::ControlText;
			this->btNeptune->Location = System::Drawing::Point(1094, 597);
			this->btNeptune->Name = L"btNeptune";
			this->btNeptune->Size = System::Drawing::Size(158, 23);
			this->btNeptune->TabIndex = 49;
			this->btNeptune->TabStop = false;
			this->btNeptune->Text = L"Neptune";
			this->btNeptune->UseVisualStyleBackColor = true;
			this->btNeptune->Click += gcnew System::EventHandler(this, &PlanetLab::btNeptune_Click);
			// 
			// btUranus
			// 
			this->btUranus->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btUranus->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btUranus->Enabled = false;
			this->btUranus->Location = System::Drawing::Point(1094, 568);
			this->btUranus->Name = L"btUranus";
			this->btUranus->Size = System::Drawing::Size(158, 23);
			this->btUranus->TabIndex = 48;
			this->btUranus->TabStop = false;
			this->btUranus->Text = L"Uranus";
			this->btUranus->UseVisualStyleBackColor = true;
			this->btUranus->Click += gcnew System::EventHandler(this, &PlanetLab::btUranus_Click);
			// 
			// btSaturn
			// 
			this->btSaturn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btSaturn->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btSaturn->Enabled = false;
			this->btSaturn->Location = System::Drawing::Point(1094, 539);
			this->btSaturn->Name = L"btSaturn";
			this->btSaturn->Size = System::Drawing::Size(158, 23);
			this->btSaturn->TabIndex = 47;
			this->btSaturn->TabStop = false;
			this->btSaturn->Text = L"Saturn";
			this->btSaturn->UseVisualStyleBackColor = true;
			this->btSaturn->Click += gcnew System::EventHandler(this, &PlanetLab::btSaturn_Click);
			// 
			// btJupiter
			// 
			this->btJupiter->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btJupiter->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btJupiter->Enabled = false;
			this->btJupiter->Location = System::Drawing::Point(1094, 510);
			this->btJupiter->Name = L"btJupiter";
			this->btJupiter->Size = System::Drawing::Size(158, 23);
			this->btJupiter->TabIndex = 46;
			this->btJupiter->TabStop = false;
			this->btJupiter->Text = L"Jupiter";
			this->btJupiter->UseVisualStyleBackColor = true;
			this->btJupiter->Click += gcnew System::EventHandler(this, &PlanetLab::btJupiter_Click);
			// 
			// btMars
			// 
			this->btMars->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btMars->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btMars->Enabled = false;
			this->btMars->Location = System::Drawing::Point(1094, 481);
			this->btMars->Name = L"btMars";
			this->btMars->Size = System::Drawing::Size(158, 23);
			this->btMars->TabIndex = 45;
			this->btMars->TabStop = false;
			this->btMars->Text = L"Mars";
			this->btMars->UseVisualStyleBackColor = true;
			this->btMars->Click += gcnew System::EventHandler(this, &PlanetLab::btMars_Click);
			// 
			// btEarth
			// 
			this->btEarth->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btEarth->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btEarth->Enabled = false;
			this->btEarth->Location = System::Drawing::Point(1094, 452);
			this->btEarth->Name = L"btEarth";
			this->btEarth->Size = System::Drawing::Size(158, 23);
			this->btEarth->TabIndex = 44;
			this->btEarth->TabStop = false;
			this->btEarth->Text = L"Earth";
			this->btEarth->UseVisualStyleBackColor = true;
			this->btEarth->Click += gcnew System::EventHandler(this, &PlanetLab::btEarth_Click);
			// 
			// btVenus
			// 
			this->btVenus->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btVenus->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->btVenus->Enabled = false;
			this->btVenus->Location = System::Drawing::Point(1094, 423);
			this->btVenus->Name = L"btVenus";
			this->btVenus->Size = System::Drawing::Size(158, 23);
			this->btVenus->TabIndex = 43;
			this->btVenus->TabStop = false;
			this->btVenus->Text = L"Venus";
			this->btVenus->UseVisualStyleBackColor = true;
			this->btVenus->Click += gcnew System::EventHandler(this, &PlanetLab::btVenus_Click);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label1->ForeColor = System::Drawing::SystemColors::Control;
			this->label1->Location = System::Drawing::Point(1169, 17);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(78, 13);
			this->label1->TabIndex = 26;
			this->label1->Text = L"Chosen planet:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetName
			// 
			this->planetName->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetName->ForeColor = System::Drawing::SystemColors::Control;
			this->planetName->Location = System::Drawing::Point(1082, 30);
			this->planetName->Name = L"planetName";
			this->planetName->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->planetName->Size = System::Drawing::Size(170, 26);
			this->planetName->TabIndex = 27;
			this->planetName->Text = L"%planetName%";
			this->planetName->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label3->ForeColor = System::Drawing::SystemColors::Control;
			this->label3->Location = System::Drawing::Point(1189, 66);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(58, 13);
			this->label3->TabIndex = 28;
			this->label3->Text = L"Total area:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetArea
			// 
			this->planetArea->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetArea->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetArea->ForeColor = System::Drawing::SystemColors::Control;
			this->planetArea->Location = System::Drawing::Point(1095, 79);
			this->planetArea->Name = L"planetArea";
			this->planetArea->Size = System::Drawing::Size(157, 26);
			this->planetArea->TabIndex = 29;
			this->planetArea->Text = L"%planetArea%";
			this->planetArea->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label6
			// 
			this->label6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label6->ForeColor = System::Drawing::SystemColors::Control;
			this->label6->Location = System::Drawing::Point(1204, 115);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(43, 13);
			this->label6->TabIndex = 30;
			this->label6->Text = L"Radius:";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetRadius
			// 
			this->planetRadius->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetRadius->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetRadius->ForeColor = System::Drawing::SystemColors::Control;
			this->planetRadius->Location = System::Drawing::Point(1073, 128);
			this->planetRadius->Name = L"planetRadius";
			this->planetRadius->Size = System::Drawing::Size(179, 26);
			this->planetRadius->TabIndex = 31;
			this->planetRadius->Text = L"%planetRadius%";
			this->planetRadius->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label8
			// 
			this->label8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label8->ForeColor = System::Drawing::SystemColors::Control;
			this->label8->Location = System::Drawing::Point(1212, 163);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(35, 13);
			this->label8->TabIndex = 32;
			this->label8->Text = L"Mass:";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetMass
			// 
			this->planetMass->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetMass->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetMass->ForeColor = System::Drawing::SystemColors::Control;
			this->planetMass->Location = System::Drawing::Point(1084, 176);
			this->planetMass->Name = L"planetMass";
			this->planetMass->Size = System::Drawing::Size(163, 26);
			this->planetMass->TabIndex = 33;
			this->planetMass->Text = L"%planetMass%";
			this->planetMass->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetSHI
			// 
			this->planetSHI->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetSHI->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetSHI->ForeColor = System::Drawing::SystemColors::Control;
			this->planetSHI->Location = System::Drawing::Point(1099, 358);
			this->planetSHI->Name = L"planetSHI";
			this->planetSHI->Size = System::Drawing::Size(148, 26);
			this->planetSHI->TabIndex = 41;
			this->planetSHI->Text = L"%planetSHI%";
			this->planetSHI->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label10
			// 
			this->label10->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label10->ForeColor = System::Drawing::SystemColors::Control;
			this->label10->Location = System::Drawing::Point(1202, 211);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(45, 13);
			this->label10->TabIndex = 34;
			this->label10->Text = L"Density:";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label16
			// 
			this->label16->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label16->ForeColor = System::Drawing::SystemColors::Control;
			this->label16->Location = System::Drawing::Point(1096, 345);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(151, 13);
			this->label16->TabIndex = 40;
			this->label16->Text = L"Rate of silicate/hydrogen/iron:";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetDensity
			// 
			this->planetDensity->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetDensity->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetDensity->ForeColor = System::Drawing::SystemColors::Control;
			this->planetDensity->Location = System::Drawing::Point(1063, 224);
			this->planetDensity->Name = L"planetDensity";
			this->planetDensity->Size = System::Drawing::Size(184, 26);
			this->planetDensity->TabIndex = 35;
			this->planetDensity->Text = L"%planetDensity%";
			this->planetDensity->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetAtmo
			// 
			this->planetAtmo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetAtmo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetAtmo->ForeColor = System::Drawing::SystemColors::Control;
			this->planetAtmo->Location = System::Drawing::Point(1084, 315);
			this->planetAtmo->Name = L"planetAtmo";
			this->planetAtmo->Size = System::Drawing::Size(163, 26);
			this->planetAtmo->TabIndex = 39;
			this->planetAtmo->Text = L"%planetAtmo%";
			this->planetAtmo->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label12
			// 
			this->label12->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label12->ForeColor = System::Drawing::SystemColors::Control;
			this->label12->Location = System::Drawing::Point(1114, 258);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(133, 13);
			this->label12->TabIndex = 36;
			this->label12->Text = L"Distance from nearest star:";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label14
			// 
			this->label14->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label14->ForeColor = System::Drawing::SystemColors::Control;
			this->label14->Location = System::Drawing::Point(1181, 302);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(66, 13);
			this->label14->TabIndex = 38;
			this->label14->Text = L"Atmosphere:";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// planetSunDist
			// 
			this->planetSunDist->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->planetSunDist->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.25F));
			this->planetSunDist->ForeColor = System::Drawing::SystemColors::Control;
			this->planetSunDist->Location = System::Drawing::Point(1059, 271);
			this->planetSunDist->Name = L"planetSunDist";
			this->planetSunDist->Size = System::Drawing::Size(188, 26);
			this->planetSunDist->TabIndex = 37;
			this->planetSunDist->Text = L"%planetSunDist%";
			this->planetSunDist->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// btPanel
			// 
			this->btPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btPanel->BackColor = System::Drawing::Color::Transparent;
			this->btPanel->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->btPanel->Location = System::Drawing::Point(1078, 387);
			this->btPanel->Name = L"btPanel";
			this->btPanel->Size = System::Drawing::Size(181, 291);
			this->btPanel->TabIndex = 51;
			this->btPanel->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &PlanetLab::btPanel_MouseClick);
			// 
			// PlanetLab
			// 
			this->AutoValidate = System::Windows::Forms::AutoValidate::EnablePreventFocusChange;
			this->BackColor = System::Drawing::SystemColors::Desktop;
			this->ClientSize = System::Drawing::Size(1264, 681);
			this->Controls->Add(this->btExit);
			this->Controls->Add(this->btMercury);
			this->Controls->Add(this->window);
			this->Controls->Add(this->btNeptune);
			this->Controls->Add(this->btUranus);
			this->Controls->Add(this->planetSunDist);
			this->Controls->Add(this->btSaturn);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->btJupiter);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->btMars);
			this->Controls->Add(this->planetAtmo);
			this->Controls->Add(this->btEarth);
			this->Controls->Add(this->planetDensity);
			this->Controls->Add(this->btVenus);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->planetName);
			this->Controls->Add(this->planetSHI);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->planetMass);
			this->Controls->Add(this->planetArea);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->planetRadius);
			this->Controls->Add(this->btPanel);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximumSize = System::Drawing::Size(1280, 720);
			this->MinimumSize = System::Drawing::Size(1280, 720);
			this->Name = L"PlanetLab";
			this->Text = L"PlanetLab";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &PlanetLab::PlanetLab_FormClosing);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &PlanetLab::window_Paint);
			this->ResumeLayout(false);

		}

	private: System::Void window_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
	{
		render_frame();
	}
	private: System::Void btMercury_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 0;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btVenus_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 1;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btEarth_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 2;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btMars_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 3;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btJupiter_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 4;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btSaturn_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 5;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btUranus_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 6;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btNeptune_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Shade(currentplanet);
		currentplanet = 7;
		Hightlight(currentplanet);
		RefreshValues();
	}
	private: System::Void btExit_Click(System::Object^  sender, System::EventArgs^  e)
	{
		this->Close();
	}

	private: System::Void btPanel_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		btMercury->Enabled = !btMercury->Enabled;
		btVenus->Enabled = !btVenus->Enabled;;
		btEarth->Enabled = !btEarth->Enabled;;
		btMars->Enabled = !btMars->Enabled;;
		btJupiter->Enabled = !btJupiter->Enabled;
		btSaturn->Enabled = !btSaturn->Enabled;
		btUranus->Enabled = !btUranus->Enabled;
		btNeptune->Enabled = !btNeptune->Enabled;
		btExit->Enabled = !btExit->Enabled;
	}
	private: System::Void PlanetLab_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
	{
		for (int i = 0; i < 10; i++)
		{
			if (Planets[i].texture)
			{
				Planets[i].texture->Release();
			}
			Planets[i].mesh->Release();
		}
		cleanD3D();
	}
	};
}
// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics();    // call the function to initialize the cube

	//d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);	// turn on the z-buffer
	d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light
	d3ddev->SetRenderState(D3DRS_WRAP0, D3DWRAPCOORD_0);
}

// this is the function used to render a single frame
bool spacePressed = false;
void render_frame(void)
{
	float zoomUnits = 5;
	float turnAngle = 5;

	if (GetAsyncKeyState(VK_SPACE))
	{
		if (!spacePressed)
		{
			rotate = !rotate;
			spacePressed = true;
		}		
	}
	else
	{
		spacePressed = false;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		CameraUpDown(-turnAngle);
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		CameraUpDown(turnAngle);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		CameraLeftRight(-turnAngle);
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		CameraLeftRight(turnAngle);
	}
	if (GetAsyncKeyState(VK_ADD))
	{
		CameraZoom(-zoomUnits);
	}
	else if (GetAsyncKeyState(VK_SUBTRACT))
	{
		CameraZoom(zoomUnits);
	}
	if (GetAsyncKeyState(0x4D))
	{
		SwapTexture();
	}

	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();

	// set the view transform
	D3DXMATRIX matView, matRotateY, matRotateX, matRotateZ, matTranslate;

	D3DXMatrixLookAtLH(&matView,
		&camPos,    // the camera position
		&camLookAt,      // the look-at position
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
	d3ddev->SetTransform(D3DTS_VIEW, &matView);

	// set the projection transform
	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),
		(FLOAT)width / (FLOAT)height,
		1.0f,    // the near view-plane
		40001.0f);    // the far view-plane
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);

	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Planets[9].speed -= Planets[9].speedIncrement;
	D3DXMatrixTranslation(&matTranslate, Planets[9].distanceFromSun, 0.0f, 0.0f);
	D3DXMatrixRotationY(&matRotateY, Planets[9].speed);
	d3ddev->SetTransform(D3DTS_WORLD, &(matTranslate * matRotateY));
	d3ddev->SetTexture(0, Planets[9].texture);
	d3ddev->SetMaterial(&Planets[9].material);
	Planets[9].mesh->DrawSubset(NULL);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	for (int i = 0; i < 9; i++)
	{
		if (rotate)
		{
			Planets[i].speed -= Planets[i].speedIncrement;
		}
		D3DXMatrixTranslation(&matTranslate, Planets[i].distanceFromSun, 0.0f, 0.0f);
		D3DXMatrixRotationY(&matRotateY, Planets[i].speed);
		d3ddev->SetTransform(D3DTS_WORLD, &(matTranslate * matRotateY));
		d3ddev->SetTexture(0, Planets[i].texture);
		d3ddev->SetMaterial(&Planets[i].material);
		Planets[i].mesh->DrawSubset(NULL);
	}

	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	d3ddev->Release();
	d3d->Release();
}

// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
	camLookAt.x = 0;
	camLookAt.y = 0;
	camLookAt.z = 0;

	camPos.x = 0;
	camPos.y = 200;
	camPos.z = 750;

	Planets[0].setup(69.601f * 2, 0.0f, -0.01f, "Textures/Sun.jpg", 1.0f);
	Planets[1].setup(2.244f, 58.0f + 90, 0.04788f, "Textures/Mercury.jpg", 0.1f);
	Planets[2].setup(6.052f, 108.0f + 90, 0.03502f, "Textures/Venus.jpg", 0.1f);
	Planets[3].setup(6.0378f, 150.0f + 90, 0.02979f, "Textures/Earth.jpg", 0.1f);
	Planets[4].setup(3.396f, 228.0f + 90, 0.02414f, "Textures/Mars.jpg", 0.1f);
	Planets[5].setup(69.911f, 778.0f, 0.01306f, "Textures/Jupiter.jpg", 0.1f);
	Planets[6].setup(58.232f, 1433.0f, 0.00964f, "Textures/Saturn.jpg", 0.1f);
	Planets[7].setup(25.362f, 2870.0f, 0.0068f, "Textures/Uranus.jpg", 0.1f);
	Planets[8].setup(24.622f, 4498.0f, 0.00543f, "Textures/Neptune.jpg", 0.1f);
	Planets[9].setup(20000.0f, 0.0f, -0.0001f, "Textures/Background2.jpg", 1.0f);
}