#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <xinput.h>
#include "sfcommon.h"
#include "sfobject.h"
#include "african_face_obj.h"
// #include "cube_obj.h"
// #include "simple_house_obj.h"
#include "sfmath/sfmath.h"
#include "sfplot.h"

#define WIDTH 800
#define HEIGHT 600

struct Controller {
	int up;
	int down;
	int left;
	int right;

	int start;
	int back;

	int lthumb;
	int rthumb;

	int lshoulder;
	int rshoulder;

	int a;
	int b;
	int x;
	int y;

	char ltrigger;
	char rtrigger;

	short int lthumb_x;
	short int lthumb_y;

	short int rthumb_x;
	short int rthumb_y;
};

typedef struct Controller controller;

bool window_running = true;

char DebugStrBuffer[256];

static BITMAPINFO bitmap_info;
static HBITMAP bitmap_handle;
static HDC device_context;

// void GetControllerState(Controller *controller, XINPUT_GAMEPAD *pad) {
// 	controller->up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
// 	controller->down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
// 	controller->left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
// 	controller->right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
// 
// 	controller->start = (pad->wButtons & XINPUT_GAMEPAD_START);
// 	controller->back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
// 
// 	controller->lthumb = (pad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
// 	controller->rthumb = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
// 
// 	controller->lshoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
// 	controller->rshoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
// 
// 	controller->a = (pad->wButtons & XINPUT_GAMEPAD_A);
// 	controller->b = (pad->wButtons & XINPUT_GAMEPAD_B);
// 	controller->x = (pad->wButtons & XINPUT_GAMEPAD_X);
// 	controller->y = (pad->wButtons & XINPUT_GAMEPAD_Y);
// }

// void HandleControllerInput() {
// 	unsigned int dwResult;
// 
// 	for (DWORD i=0; i< XUSER_MAX_COUNT; i++ )
// 	{
// 		XINPUT_STATE controller_state;
// 		ZeroMemory(&controller_state, sizeof(XINPUT_STATE) );
// 
// 		// Simply get the state of the controller from XInput.
// 		dwResult = XInputGetState(i, &controller_state);
// 
// 		if( dwResult == ERROR_SUCCESS )
// 		{
// 			// Controller is connected
// 			XINPUT_GAMEPAD *pad = &controller_state.Gamepad;
// 
// 			GetControllerState(&controller, pad);
// 		}
// 		else
// 		{
// 			// Controller is not connected
// 		}
// 	}
// }

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg,
							WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					PWSTR pCmdLine, int nCmdShow)
{

	bitmap.width = WIDTH;
	bitmap.height = HEIGHT;
	bitmap.bpp = 4;

	const char *class_name = "Window Class";

	WNDCLASSEX window_class = {0};

	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = WindowProc;
	window_class.hInstance = hInstance;
	window_class.lpszClassName = class_name;

	bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
	bitmap_info.bmiHeader.biPlanes = 1;
	bitmap_info.bmiHeader.biBitCount = (bitmap.bpp * 8);
	bitmap_info.bmiHeader.biCompression = BI_RGB;

	device_context = CreateCompatibleDC(0);

	if(!RegisterClassEx(&window_class)) {
		return 0;
	}

	HWND window_handle = CreateWindowEx(0, class_name,
										"Windows Program",
										WS_OVERLAPPEDWINDOW,
										CW_USEDEFAULT, CW_USEDEFAULT,
										bitmap.width, bitmap.height,
										NULL, NULL, hInstance, NULL);

	if (window_handle == NULL)
	{
		return 0;
	}

	RECT client_rect;
	GetClientRect(window_handle, &client_rect);

	ShowWindow(window_handle, nCmdShow);
	UpdateWindow(window_handle);

	MSG msg = {0};

	float *z_buffer = (float *)VirtualAlloc(0, (WIDTH * HEIGHT * 4),
											MEM_COMMIT|MEM_RESERVE,
											PAGE_READWRITE);

	for (int i = 0; i < (WIDTH * HEIGHT); i++) {
		// unsigned int n = 0xff7fffff;
		unsigned int n = 0x7f7fffff;
		z_buffer[i] = TYPE_PUN(n, float);
	}

	mat4 view_matrix;
	vec3f32 camera_dir = {
		.x = 0.0f,
		.y = 0.0f,
		.z = -1.0f,
	};
	vec3f32 camera_up = {
		.x = 0.0f,
		.y = 1.0f,
		.z = 0.0f,
	};
	vec3f32 camera_pos = {
		.x = 0.0f,
		.y = 0.0f,
		.z = 0.0f,
	};

	mat4_gen_lookat(camera_dir, camera_up, camera_pos, view_matrix);

	float nearVal = 0.1f;
	float farVal = 100.0f;
	float fov = PI / 2;
	float client_width = client_rect.right - client_rect.left;
	float client_height = client_rect.bottom - client_rect.top;
	float aspect_ratio = client_width / client_height;
	mat4 proj_matrix;

	mat4_gen_perspective_proj(nearVal, farVal, fov, aspect_ratio, proj_matrix);

	mat4 mvp;
	mat4_multiply(proj_matrix, view_matrix, mvp);

	while (window_running)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				window_running = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (bitmap.memory)
		{
			ZeroMemory(bitmap.memory, (bitmap.width * bitmap.height * bitmap.bpp));
		}

		unsigned int faces_size = sizeof(faces)/sizeof(faces[0]);
		for (int i = 0; i < faces_size; i+=3) {

			int vertex_index[3];
			vertex_index[0] = faces[i].vI - 1;
			vertex_index[1] = faces[i + 1].vI - 1;
			vertex_index[2] = faces[i + 2].vI - 1;

			vec3f32 world_vertex[3];

			vec3i32 screen_vertex[3];

			vec3f32 proj_vertex[3];

			world_vertex[0].x = (vertex_geometric[vertex_index[0]].x);
			world_vertex[0].y = (vertex_geometric[vertex_index[0]].y);
			world_vertex[0].z = (vertex_geometric[vertex_index[0]].z);

			world_vertex[1].x = (vertex_geometric[vertex_index[1]].x);
			world_vertex[1].y = (vertex_geometric[vertex_index[1]].y);
			world_vertex[1].z = (vertex_geometric[vertex_index[1]].z);

			world_vertex[2].x = (vertex_geometric[vertex_index[2]].x);
			world_vertex[2].y = (vertex_geometric[vertex_index[2]].y);
			world_vertex[2].z = (vertex_geometric[vertex_index[2]].z);

			vec3f32 light_dir = {
				.x = 0.0f,
				.y = 0.0f,
				.z = -1.0f,
			};

			vec3f32 normal;
			vec3f32 v1 = {
				.x = world_vertex[2].x - world_vertex[0].x,
				.y = world_vertex[2].y - world_vertex[0].y,
				.z = world_vertex[2].z - world_vertex[0].z,
			};
			vec3f32 v2 = {
				.x = world_vertex[1].x - world_vertex[0].x,
				.y = world_vertex[1].y - world_vertex[0].y,
				.z = world_vertex[1].z - world_vertex[0].z,
			};
			vec3_cross(&v1, &v2, &normal);
			vec3_normalize(&normal, &normal);

			float intensity = vec3_dot(&normal, &light_dir);

			RGBAColor colors[3] = {
				{
					.r = 0xff * intensity,
					.g = 0xff * intensity,
					.b = 0xff * intensity,
					.a = 0xff * intensity,
				},
				{
					.r = 0xff * intensity,
					.g = 0xff * intensity,
					.b = 0xff * intensity,
					.a = 0xff * intensity,
				},
				{
					.r = 0xff * intensity,
					.g = 0xff * intensity,
					.b = 0xff * intensity,
					.a = 0xff * intensity,
				},
			};

			float w = 1.0f;
			float proj_w[3];

			proj_w[0] = mat4_mult_vec3(proj_matrix, world_vertex[0], &proj_vertex[0], w);
			proj_w[1] = mat4_mult_vec3(proj_matrix, world_vertex[1], &proj_vertex[1], w);
			proj_w[2] = mat4_mult_vec3(proj_matrix, world_vertex[2], &proj_vertex[2], w);


			screen_vertex[0].x = ((proj_vertex[0].x + 1)/2 * (bitmap.width));
			screen_vertex[0].y = ((proj_vertex[0].y + 1)/2 * (bitmap.height));
			screen_vertex[0].z = TYPE_PUN(proj_vertex[0].z, int);

			screen_vertex[1].x = ((proj_vertex[1].x + 1)/2 * (bitmap.width));
			screen_vertex[1].y = ((proj_vertex[1].y + 1)/2 * (bitmap.height));
			screen_vertex[1].z = TYPE_PUN(proj_vertex[1].z, int);

			screen_vertex[2].x = ((proj_vertex[2].x + 1)/2 * (bitmap.width));
			screen_vertex[2].y = ((proj_vertex[2].y + 1)/2 * (bitmap.height));
			screen_vertex[2].z = TYPE_PUN(proj_vertex[2].z, int);

			plotFilledTriangle(screen_vertex, z_buffer, &colors);
		}

		InvalidateRect(window_handle, NULL, FALSE);
		UpdateWindow(window_handle);

	}

	VirtualFree(z_buffer, WIDTH * HEIGHT * 4, MEM_DECOMMIT|MEM_RELEASE);
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg,
							WPARAM wParam, LPARAM lParam)
{
	LRESULT result;
	PAINTSTRUCT paint;

	switch(uMsg)
	{
	case WM_PAINT:
		{
			HDC paint_device_context = BeginPaint(hwnd, &paint);
			BitBlt(paint_device_context, paint.rcPaint.left,
				   paint.rcPaint.top,
				   paint.rcPaint.right - paint.rcPaint.left,
				   paint.rcPaint.bottom - paint.rcPaint.top,
				   device_context, paint.rcPaint.left, paint.rcPaint.top,
				   SRCCOPY);
			EndPaint(hwnd, &paint);
			break;
		}

	case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

	case WM_SIZE:
		{
			bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
			bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

			bitmap.width = bitmap_info.bmiHeader.biWidth;
			bitmap.height = bitmap_info.bmiHeader.biHeight;

			if (bitmap_handle) DeleteObject(bitmap_handle);

			bitmap_handle = CreateDIBSection(0, &bitmap_info, DIB_RGB_COLORS,
											 (void **)&bitmap.memory, 0, 0);
			SelectObject(device_context, bitmap_handle);
		}

	default:
		{
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		}
	}

	return result;
}
