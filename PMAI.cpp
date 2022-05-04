// PM.cpp : Defines the entry point for the application.
//

#include "WorldController.h"
#include "NEAT\CController.h"
#include "Utilities\CTimer.h"
#include "Utilities\ParameterMap.h"
#include "Brain_Keyboard.h"

#include <time.h>

//globals
HWND g_hwndInfo = NULL;
HWND g_hwndMain = NULL;
HWND g_hwndConsole = NULL;

fstream logfile;

CController*	g_pController	 = NULL;
CParams         g_Params; //this is the parameter set for NEAT - no need for a file as the ParameterMap will take care of it
ParameterMap    g_AppParams("AIPac.param");
string g_PlayMode = g_AppParams.GetStringParam("Mode");

//variables for Play mode
WorldController *g_pGameEngine;
IBrain *g_pPlayBrain;

//forward declares
LRESULT CALLBACK WindowProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ConsoleWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void CreateInfoWindow(HWND hwndParent);
void CreateConsoleWindow(HWND hwndParent);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//put these here for ease of changing later
	char *pApplicationName = "PMAI";
	char *pWindowClassName = "PMAI";

	HWND hWnd; //window handle
	MSG msg;  //message for message pump

	CTimer timer(10);
	bool g_bDone = false;	//monitor continuation status for primary loop
    bool playReturn; //for PLAY mode, monitor for finishing the game

	g_Params.Initialize();

 	//create a window that we can draw on
	WNDCLASSEX winclass;

	winclass.cbSize		=	sizeof(WNDCLASSEX);
	winclass.style		=	CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc=	WindowProc;
	winclass.cbClsExtra	=	0;
	winclass.cbWndExtra	=	0;
	winclass.hInstance	=	hInstance;
	winclass.hIcon		=	LoadIcon(NULL,IDI_APPLICATION);
	winclass.hCursor	=	LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground	=	NULL;
	winclass.lpszMenuName	=	NULL;
	winclass.lpszClassName	=	pWindowClassName;
	winclass.hIconSm		=	LoadIcon(NULL, IDI_APPLICATION);

	//register the window class
	if (!RegisterClassEx(&winclass))
	{
		MessageBox(NULL, "Class Registration Failed!", "Error", 0);
		return 0;
	}

	hWnd = CreateWindowEx(	NULL,
							pWindowClassName,
							pApplicationName,
							WS_OVERLAPPEDWINDOW,
							0,0,
							g_AppParams.GetIntParam("WindowHeight"),
							g_AppParams.GetIntParam("WindowHeight"),
							NULL, NULL,
							hInstance,
							NULL);

    g_hwndMain = hWnd;

    //make the window visible
    if ( g_PlayMode != "Play" )
    {
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        //create and show the info window
        CreateInfoWindow(hWnd);
    }

    //create console window
    CreateConsoleWindow(hWnd);

	if (!hWnd)
	{
		MessageBox (NULL, "CreateWindowEx Failed!", "Error", 0);
	}

	//set the timer
	timer.Start();

	while (!g_bDone)
	{
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			if ( msg.message == WM_QUIT)
				g_bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

        if ( g_PlayMode == "Evolve" )
        {
            if(!g_pController->Update())
            {
                //we have a problem, end app
                g_bDone = TRUE;
            }

            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }

        if ( g_PlayMode == "Play" )
        {
            playReturn = g_pGameEngine->Update(false);
            Sleep(g_AppParams.GetLongParam("VisDelay"));
            if ( playReturn )
            {
                InvalidateRect(g_hwndConsole, NULL, TRUE);
                UpdateWindow(g_hwndConsole);
            }

            if ( !playReturn )
                g_pGameEngine->Reset();
        }
	}

	//unregister the class
	UnregisterClass(pWindowClassName, winclass.hInstance);

	return 0;
}


LRESULT CALLBACK WindowProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//static WorldController GameEngine("AIPac.param");
    static int cxClient, cyClient;

	switch (msg)
	{
	    //check key press messages
		case WM_KEYUP:
		{
			switch(wParam)
			{

				case VK_ESCAPE:
				{
					PostQuitMessage(0);
				}
				break;

				case 'B':
                {
                    if ( g_PlayMode == "Evolve" )
                    {
                        g_pController->RenderBestToggle();

                        if ( g_pController->RenderBest() )
                            SetWindowText( g_hwndConsole, "PM Visualizer - Active" );
                        else
                            SetWindowText( g_hwndConsole, "PM Visualizer - Sleeping" );
                    }
                    break;
                }

            }//end WM_KEYUP switch
        }
        break;

		case WM_CREATE:
		{
//		    long s = time(NULL);
//		    ofstream logfile;
//		    logfile.open("seed.log", ios::out);
//		    logfile << "Seed: " << s << endl;
//		    logfile.close();
//		    srand( 1294863823 );
		    srand( time(NULL) );

		    RECT rect;
			GetClientRect(hWnd, &rect);
			cxClient = rect.right;
			cyClient = rect.bottom;
			if ( g_PlayMode == "Evolve" )
                g_pController = new CController(hWnd, cxClient, cyClient);
            else if ( g_PlayMode == "Play" )
            {
                g_pGameEngine = new WorldController("AIPac.param");
                g_pPlayBrain = new Brain_Keyboard();
                g_pGameEngine->SetPacBrain(g_pPlayBrain);
            }
			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);

			//fill our backbuffer with white
			BitBlt(hdc,
             0,
             0,
             cxClient,
             cyClient,
             NULL,
             NULL,
             NULL,
             WHITENESS);

            if ( g_PlayMode == "Evolve" )
                g_pController->Render(hdc);

			//tell windows we're done
			EndPaint(hWnd, &ps);
			return 0;
		}
	}

	return DefWindowProc (hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK InfoWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//these hold the dimensions of the client window area
	static int cxinfoClient, cyinfoClient;

	switch(msg)
	{
	    //check key press messages
		case WM_KEYUP:
		{
			switch(wparam)
			{

				case VK_ESCAPE:
				{
					PostQuitMessage(0);
				}
				break;

				case 'B':
				{
                    g_pController->RenderBestToggle();

                    if ( g_pController->RenderBest() )
                        SetWindowText( g_hwndConsole, "PM Visualizer - Active" );
                    else
                        SetWindowText( g_hwndConsole, "PM Visualizer - Sleeping" );

                    break;
				}

            }//end WM_KEYUP switch
        }
        break;

		case WM_CREATE:
		{
			//get the size of the client window
			RECT rect;
			GetClientRect(hwnd, &rect);

			cxinfoClient = rect.right;
			cyinfoClient = rect.bottom;
		}

		break;

		//has the user resized the client area?
		case WM_SIZE:
		{
 			cxinfoClient = LOWORD(lparam);
			cyinfoClient = HIWORD(lparam);
		}

		break;

		case WM_PAINT:
		{
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            g_pController->RenderNetworks(ps.hdc);
			EndPaint(hwnd, &ps);
		}

		break;

		case WM_CLOSE:
		{
		    exit(0);
		}

		break;

		default:break;

	}//end switch

	// default msg handler
	return DefWindowProc (hwnd, msg, wparam, lparam);

}//end WinProc
//---------------------------------CreateInfoWindow---------------------------
//
//	creates and displays the info window
//
//----------------------------------------------------------------------------
void CreateInfoWindow(HWND hwndParent)
{
	// Create and register the window class
  WNDCLASSEX wcInfo = {sizeof(WNDCLASSEX),
                       CS_HREDRAW | CS_VREDRAW,
                       InfoWindowProc,
                       0,
                       0,
                       GetModuleHandle(NULL),
						           NULL,
						           NULL,
						           (HBRUSH)(GetStockObject(WHITE_BRUSH)),
						           NULL,
						           "Info",
						           NULL };

  RegisterClassEx( &wcInfo );

	// Create the application's info window
  g_hwndInfo = CreateWindow("Info",
                            "Previous generation's best four phenotypes",
                            WS_OVERLAPPEDWINDOW,
                            500,
                            0,
                            g_AppParams.GetLongParam("InfoWindowHeight"),
                            g_AppParams.GetLongParam("InfoWindowWidth"),
									          hwndParent,
									          NULL,
									          wcInfo.hInstance,
									          NULL );

	// Show the info
	ShowWindow(g_hwndInfo, SW_SHOW);
	UpdateWindow(g_hwndInfo);

  //give the info window's handle to the controller
  g_pController->PassInfoHandle(g_hwndInfo);

	return;
}

void CreateConsoleWindow(HWND hwndParent)
{
    //create a window that we can draw on
	WNDCLASSEX winclass;

	winclass.cbSize		=	sizeof(WNDCLASSEX);
	winclass.style		=	CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc=	ConsoleWindowProc;
	winclass.cbClsExtra	=	0;
	winclass.cbWndExtra	=	0;
	winclass.hInstance	=	GetModuleHandle(NULL);
	winclass.hIcon		=	NULL;
	winclass.hCursor	=	NULL;
	winclass.hbrBackground	=	NULL;
	winclass.lpszMenuName	=	NULL;
	winclass.lpszClassName	=	"Console";
	winclass.hIconSm		=	NULL;

	//register the window class
	if (!RegisterClassEx(&winclass))
	{
		MessageBox(NULL, "Class Registration Failed!", "Error", 0);
		return;
	}

	g_hwndConsole = CreateWindow("Console",
							"PM Visualizer",
							WS_OVERLAPPEDWINDOW,
							0,400,
							472,544,
							hwndParent, NULL,
							winclass.hInstance,
							NULL);

	// Show the info
	ShowWindow(g_hwndConsole, SW_SHOW);
	UpdateWindow(g_hwndConsole);

  //give the info window's handle to the controller
  if ( g_PlayMode == "Evolve" )
    g_pController->PassConsoleHandle(g_hwndConsole);

	return;
}

LRESULT CALLBACK ConsoleWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//these hold the dimensions of the client window area
	static int cxClient, cyClient;

	switch(msg)
	{
	    //check key press messages
		case WM_KEYUP:
		{
			switch(wparam)
			{
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
				}
				break;

				case 'B':
                {
                    if ( g_PlayMode == "Evolve" )
                    {
                        g_pController->RenderBestToggle();

                        if ( g_pController->RenderBest() )
                            SetWindowText( g_hwndConsole, "PM Visualizer - Active" );
                        else
                            SetWindowText( g_hwndConsole, "PM Visualizer - Sleeping" );
                        break;
                    }
                }
            }//end WM_KEYUP switch
        }
        break;

		case WM_CREATE:
		{
			//get the size of the client window
			RECT rect;
			GetClientRect(hwnd, &rect);

			cxClient = rect.right;
			cyClient = rect.bottom;
		}

		break;

		//has the user resized the client area?
		case WM_SIZE:
		{
 			cxClient = LOWORD(lparam);
			cyClient = HIWORD(lparam);
		}

		break;

		case WM_PAINT:
		{
            PAINTSTRUCT ps;
            HDC hdc;
            hdc = BeginPaint(hwnd, &ps);

			//draw the world to the screen
			if ( g_PlayMode == "Evolve" )
                g_pController->DrawWorld(hdc);

            if ( g_PlayMode == "Play" )
                g_pGameEngine->Draw(hdc);

			EndPaint(hwnd, &ps);
		}

        break;

		case WM_CLOSE:
		{
		    exit(0);
		}

		break;

		default:break;

	}//end switch

	// default msg handler
	return DefWindowProc (hwnd, msg, wparam, lparam);

}
