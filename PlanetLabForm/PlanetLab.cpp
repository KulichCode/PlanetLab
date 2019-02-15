#include "PlanetLab.h"
#include <winuser.h>
#include <windows.h>
#include <windowsx.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace PlanetLabForm;

[STAThreadAttribute]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	PlanetLabForm::PlanetLab form;
	form.Show();
	while (true)
	{
		form.Invalidate();
		Application::DoEvents();
	}
	Application::Exit();
}