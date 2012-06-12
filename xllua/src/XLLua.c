/******************************************************************************
* This program and the accompanying materials
* are made available under the terms of the Common Public License v1.0
* which accompanies this distribution, and is available at 
* http://www.eclipse.org/legal/cpl-v10.html
* 
* Contributors:
*     Peter Smith
*******************************************************************************/

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <windows.h>
#include "xlcall.h"

#define RT_INI_FILE MAKEINTRESOURCE(687)
#define RES_MAGIC_SIZE 4
#define INI_RES_MAGIC MAKEFOURCC('I','N','I',' ')

// Our shared state
lua_State *l = NULL;

// Lua function stubs
void     xllua_addlibs(lua_State *l);

// Helper stubs
void     xllua_pushx(lua_State *l, LPXLOPER px);
LPXLOPER xllua_popx(lua_State *l, int idx);
void     xllua_getx(lua_State *l, int idx, LPXLOPER xl);
void     xllua_settable(lua_State *l, int idx, LPXLOPER v, BOOL *pset);
int      xllua_calli(lua_State *l, const char* fn);
int      xllua_register_function(LPXLOPER xn, const char* proc, const char* types, const char* name, const char* args, const char* mt, const char* cat, const char* sc, const char* topic, const char* fnh, const char* argh);
char*    xllua_make_excel_string(const char* string);
void     xllua_dump_stack(lua_State *L);
void     xllua_dump_xloper(LPXLOPER x);

// ------ Entry Point --------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	int s = 1;
	HRSRC hi;
	char *script;

	if(fdwReason == DLL_PROCESS_ATTACH) {
		l = lua_open();
		luaL_openlibs(l); // stdlibs
		xllua_addlibs(l);
		hi = FindResource(hinstDLL, MAKEINTRESOURCE(1), MAKEINTRESOURCE(RT_INI_FILE));
		if(hi) {
			HGLOBAL hg = LoadResource(hinstDLL, hi);
			PBYTE pb = (PBYTE) LockResource(hg);
			DWORD* pd = (DWORD*) pb;
			script = *pd == INI_RES_MAGIC ? (char *) &pb[RES_MAGIC_SIZE] : (char *) pb;
			s = luaL_dostring(l, script);	
		}
		if(s) {
			script = lua_tostring(l, -1);
			OutputDebugString(script == NULL ? "script could not be loaded\n" : script);
		} 
	} else if(fdwReason == DLL_PROCESS_DETACH && !lpvReserved) {
		if(l != NULL) {
			lua_close(l);
			l = NULL;
		}
	}
	
	return TRUE;
}

// ------ XLL Impl ------------------------------------------------------------

__declspec(dllexport) 
int WINAPI xlAutoOpen(void)
{
	int res = 0;
	XLOPER xDLL;

	// Get the name of this dll - we need it to register functions
	Excel4(xlGetName, &xDLL, 0);

	// Register to two standard functions
	xllua_register_function(&xDLL, "LuaFn", "RCPPPPPPPPPPPPPPPPPPPP",  "LF",  0, "1", "General", 0, 0, 0, 0);
	xllua_register_function(&xDLL, "LuaFn", "RCPPPPPPPPPPPPPPPPPPPP!", "LFv", 0, "1", "General", 0, 0, 0, 0);

	// Call the lua open fn (if defined)
	lua_settop(l, 0);
	lua_getglobal(l, "xllua");
	lua_getfield(l, 1, "open");
	if(!lua_isfunction(l, 2))
		return 1;
	xllua_pushx(l, &xDLL);
	lua_pcall(l, 1, 1, 0);
	if(lua_isnumber(l, -1))
		res = lua_tointeger(l, -1);
	else if(lua_isboolean(l, -1))
		res = lua_toboolean(l, -1);
	lua_settop(l, 0);

	return res;
}

__declspec(dllexport) 
int WINAPI xlAutoClose(void)
{
	return xllua_calli(l, "close");
}

__declspec(dllexport) 
LPXLOPER WINAPI xlAutoRegister(LPXLOPER pxName)
{
	LPXLOPER x;

	lua_settop(l, 0); 
	lua_getglobal(l, "xllua");
	lua_getfield(l, 1, "register");
	if(!lua_isfunction(l, 2))
		return NULL;
	xllua_pushx(l, pxName);
	lua_pcall(l, 1, 1, 0);
	x = xllua_popx(l, 1);
	lua_settop(l, 0);

	return x;
}

__declspec(dllexport) 
int WINAPI xlAutoAdd(void)
{
	return xllua_calli(l, "add");
}

__declspec(dllexport) 
int WINAPI xlAutoRemove(void)
{
	return xllua_calli(l, "remove");
}

__declspec(dllexport) 
void WINAPI xlAutoFree(LPXLOPER px)
{
	 // TODO
}

__declspec(dllexport) 
LPXLOPER WINAPI xlAddInManagerInfo(LPXLOPER xAction)
{
	return 0;
}

__declspec(dllexport) 
LPXLOPER WINAPI LuaFn(const char* name, LPXLOPER v0, LPXLOPER v1, LPXLOPER v2, LPXLOPER v3, LPXLOPER v4, 
						LPXLOPER v5, LPXLOPER v6, LPXLOPER v7, LPXLOPER v8, LPXLOPER v9, LPXLOPER v10,
						LPXLOPER v11, LPXLOPER v12, LPXLOPER v13, LPXLOPER v14, LPXLOPER v15, LPXLOPER v16,
						LPXLOPER v17, LPXLOPER v18, LPXLOPER v19)
{
	LPXLOPER x;
	BOOL set  = FALSE, *pset = &set;

	lua_settop(l, 0);
	lua_getglobal(l, "xllua");
	lua_getfield(l, 1, "fn");
	if(!lua_isfunction(l, 2))
		return NULL;
	lua_pushstring(l, name);
	lua_newtable(l);
	xllua_settable(l, 20, v19, pset);
	xllua_settable(l, 19, v18, pset);
	xllua_settable(l, 18, v17, pset);
	xllua_settable(l, 17, v16, pset);
	xllua_settable(l, 16, v15, pset);
	xllua_settable(l, 15, v14, pset);
	xllua_settable(l, 14, v13, pset);
	xllua_settable(l, 13, v12, pset);
	xllua_settable(l, 12, v11, pset);
	xllua_settable(l, 11, v10, pset);
	xllua_settable(l, 10, v9, pset);
	xllua_settable(l, 9, v8, pset);
	xllua_settable(l, 8, v7, pset);
	xllua_settable(l, 7, v6, pset);
	xllua_settable(l, 6, v5, pset);
	xllua_settable(l, 5, v4, pset);
	xllua_settable(l, 4, v3, pset);
	xllua_settable(l, 3, v2, pset);
	xllua_settable(l, 2, v1, pset);
	xllua_settable(l, 1, v0, pset);
	lua_pcall(l, 2, 1, 0);
	x = xllua_popx(l, -1);
	lua_settop(l, 0);

	return x;
}

LPXLOPER WINAPI LuaFc(int number, LPXLOPER v0, LPXLOPER v1, LPXLOPER v2, LPXLOPER v3, LPXLOPER v4, 
						LPXLOPER v5, LPXLOPER v6, LPXLOPER v7, LPXLOPER v8, LPXLOPER v9, LPXLOPER v10,
						LPXLOPER v11, LPXLOPER v12, LPXLOPER v13, LPXLOPER v14, LPXLOPER v15, LPXLOPER v16,
						LPXLOPER v17, LPXLOPER v18, LPXLOPER v19)
{
	LPXLOPER x;
	BOOL set  = FALSE, *pset = &set;

	lua_settop(l, 0);
	lua_getglobal(l, "xllua");
	lua_getfield(l, 1, "fc");
	if(!lua_isfunction(l, 2))
		return NULL;
	lua_pushinteger(l, number);
	lua_newtable(l);
	xllua_settable(l, 20, v19, pset);
	xllua_settable(l, 19, v18, pset);
	xllua_settable(l, 18, v17, pset);
	xllua_settable(l, 17, v16, pset);
	xllua_settable(l, 16, v15, pset);
	xllua_settable(l, 15, v14, pset);
	xllua_settable(l, 14, v13, pset);
	xllua_settable(l, 13, v12, pset);
	xllua_settable(l, 12, v11, pset);
	xllua_settable(l, 11, v10, pset);
	xllua_settable(l, 10, v9, pset);
	xllua_settable(l, 9, v8, pset);
	xllua_settable(l, 8, v7, pset);
	xllua_settable(l, 7, v6, pset);
	xllua_settable(l, 6, v5, pset);
	xllua_settable(l, 5, v4, pset);
	xllua_settable(l, 4, v3, pset);
	xllua_settable(l, 3, v2, pset);
	xllua_settable(l, 2, v1, pset);
	xllua_settable(l, 1, v0, pset);
	lua_pcall(l, 2, 1, 0);
	x = xllua_popx(l, -1);
	lua_settop(l, 0);

	return x;
}

// ------ Lua functions -------------------------------------------------------

int xllua_debug_print(lua_State *l)
{	
	const char* str = luaL_checkstring(l, 1);

	if(str)
		OutputDebugString(str);

	return 0;
}

int xllua_excel4(lua_State *l)
{
	int xlfn        = luaL_checkint(l, 1);
	int num_args    = luaL_checkint(l, 2);
	int rc          = 0;
	int i           = 0;
	XLOPER   x[30];
	LPXLOPER px[30];
	XLOPER   res;

	res.xltype = xltypeNil;
	for(i = 0; i < num_args; i++) {
		px[i] = &x[i];
		xllua_getx(l, i + 3, px[i]);
		xllua_dump_xloper(px[i]);
	}
	rc = Excel4v(xlfn, &res, num_args, px);
	lua_pushinteger(l, rc);
	if(rc) {
		lua_pushnil(l);
	} else {
		xllua_pushx(l, &res);
	}

	return 2;
}

int xllua_to_table(lua_State *l)
{
	return 0;
}

void xllua_addlibs(lua_State *l)
{
	static const luaL_reg fns[] = 
	{
		{ "debug_print", xllua_debug_print },
		{ "excel4",      xllua_excel4      },
		{ "to_table",    xllua_to_table    },
	    { NULL,          NULL              }
	};

	lua_newtable(l);
	lua_setglobal(l, "xllua");
	luaL_register(l, "xllua", fns);
};

// ------ Helpers -------------------------------------------------------------

void xllua_pushx(lua_State *l, LPXLOPER px)
{
	LPXLOPER* udpx;
	TCHAR dst[MAX_PATH];

	switch(px->xltype & ~(xlbitXLFree | xlbitDLLFree)) {
	case xltypeBool:
		lua_pushboolean(l, px->val.xbool);
		break;
	case xltypeErr:
		lua_pushnil(l); // FIXME
		break;
	case xltypeInt:
		lua_pushinteger(l, px->val.w);
		break;
	case xltypeMulti:
		lua_pushlightuserdata(l, px);
		break;
	case xltypeNum:
		lua_pushnumber(l, px->val.num);
		break;
	case xltypeStr:
		memcpy(dst, &px->val.str[1], (px->val.str[0]&0xff));
		dst[px->val.str[0]&0xff] = 0;
		lua_pushstring(l, dst);
		break;
	default:
		lua_pushnil(l);
	}	
}

void xllua_getx(lua_State *l, int idx, LPXLOPER xl)
{
	int xlbit   = 0;

	switch(lua_type(l, idx)) {
		case LUA_TBOOLEAN:
			xl->xltype    = xltypeBool | xlbit;
			xl->val.xbool = lua_toboolean(l, idx);
			break;
		case LUA_TSTRING:
			xl->xltype    = xltypeStr | xlbit;
			xl->val.str   = xllua_make_excel_string(lua_tostring(l, idx));
			break;
		case LUA_TNUMBER:
			xl->xltype    = xltypeNum | xlbit;
			xl->val.num   = lua_tonumber(l, idx);
			break;
		case LUA_TNIL:
		default:
			xl->xltype    = xltypeMissing | xlbit;
	}
}

LPXLOPER xllua_popx(lua_State *l, int idx)
{
	LPXLOPER xl = (LPXLOPER) malloc(sizeof(XLOPER));

	xllua_getx(l, idx, xl);

	return xl;
}

void xllua_settable(lua_State *l, int idx, LPXLOPER v, BOOL *pset)
{
	if(v == NULL || (v->xltype & ~(xlbitXLFree | xlbitDLLFree)) == xltypeMissing && !*pset)
		return;
	lua_pushinteger(l, idx);
	if(v == NULL)
		lua_pushnil(l);
	else
		xllua_pushx(l, v);
	lua_settable(l, -3);
	*pset = TRUE;
}

int xllua_calli(lua_State *l, const char* fn)
{
	int res = 0;

	lua_settop(l, 0); 
	lua_getglobal(l, "xllua");
	lua_getfield(l, 1, fn);
	if(!lua_isfunction(l, 2))
		return 1;
	lua_pcall(l, 0, 1, 0);
	if(lua_isnumber(l, -1))
		res = lua_tointeger(l, -1);
	else if(lua_isboolean(l, -1))
		res = lua_toboolean(l, -1);
	lua_settop(l, 0);

	return res;
}

int xllua_register_function(LPXLOPER xn,
	const char* proc, const char* types, const char* name,
	const char* args, const char* mt, const char* cat,
	const char* sc, const char* topic, 
	const char* fnh, const char* argh)
{
	int i, res;
	XLOPER x[11];
	LPXLOPER px[11];
	px[0] = xn;

	// Init strings
	for(i = 1; i < 11; i++) {
		x[i].val.str = NULL;
		x[i].xltype = xltypeStr;
		px[i] = &x[i];
	}

	x[1].val.str = xllua_make_excel_string(proc);
	x[2].val.str = xllua_make_excel_string(types);
	x[3].val.str = xllua_make_excel_string(name);
	x[4].val.str = xllua_make_excel_string(args);
	x[5].val.str = xllua_make_excel_string(mt);
	x[6].val.str = xllua_make_excel_string(cat);
	x[7].val.str = xllua_make_excel_string(sc);
	x[8].val.str = xllua_make_excel_string(topic);
	x[9].val.str = xllua_make_excel_string(fnh);
	x[10].val.str = xllua_make_excel_string(argh);

	// Check types for NULL
	for(i = 0; i < 11; i++) {
		if(x[i].val.str == NULL)
			x[i].xltype = xltypeMissing;
	}

	res = Excel4v(xlfRegister, 0, 11, px);

	// Free strings
	for(i = 1; i < 11; i++) {
		if(x[i].val.str != NULL)
			free(x[i].val.str);
	}

	return res;
}

char* xllua_make_excel_string(const char* string)
{
	size_t len;
	char* temp;

	if(string == NULL) return NULL;
	len = strlen(string);
	if(len > 255) len = 255; // Excel strings are limited to 255 chars
	temp = (char *) malloc(len + 2);
	memcpy(temp + 1, string, len);
	temp[0] = (BYTE) len;
	temp[len+1] = 0;

	return temp;
}

void xllua_dump_stack(lua_State *L) 
{
	char tmp[1000];
	int i;
	int top = lua_gettop(L);

	for (i = 1; i <= top; i++) {  
		int t = lua_type(L, i);
		switch (t) 
		{
			case LUA_TBOOLEAN:  
				sprintf(tmp, "d: %s\n", i, lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:  
				sprintf(tmp, "%d: %g\n", i, lua_tonumber(L, i));
				break;
			case LUA_TSTRING:  
				sprintf(tmp, "%d: `%s'\n", i, lua_tostring(L, i));
				break;
			default:  
				sprintf(tmp, "%d: type(%s)\n", i, lua_typename(L, t));
			break;
		}

		OutputDebugString(tmp);
	}
}

void xllua_dump_xloper(LPXLOPER px)
{
	char cb[MAX_PATH]; cb[0] = 0;

	switch(px->xltype & ~(xlbitXLFree | xlbitDLLFree)) {
	case xltypeBool:
		sprintf(cb, "xbool: %d\n", px->val.xbool);
		break;
	case xltypeErr:
		sprintf(cb, "err: %d\n", px->val.err);
		break;
	case xltypeInt:
		sprintf(cb, "int: %d\n", px->val.w);
		break;
	case xltypeMulti:
		sprintf(cb, "array: %d, %d\n", px->val.array.columns, px->val.array.rows);
		break;
	case xltypeNum:
		sprintf(cb, "num: %f\n", px->val.num);
		break;
	case xltypeStr:
		sprintf(cb, "str: %s\n", px->val.str == NULL ? "null" : &px->val.str[1]);
		break;
	case xltypeMissing:
		sprintf(cb, "missing\n");
		break;
	case xltypeNil:
		sprintf(cb, "nil\n");
		break;
	default:
		sprintf(cb, "unknown\n");
	}	

	OutputDebugString(cb);
}

int _cdecl _ftol2_sse()
{
	return 0;
}

// ------ LuaF# Functions -----------------------------------------------------

#define DECLARE_EXCEL_FUNCTION(number) \
__declspec(dllexport) LPXLOPER WINAPI LuaF##number (LPXLOPER v0, LPXLOPER v1, LPXLOPER v2 \
	,LPXLOPER v3, LPXLOPER v4, LPXLOPER v5, LPXLOPER v6, LPXLOPER v7, LPXLOPER v8 \
	,LPXLOPER v9, LPXLOPER v10, LPXLOPER v11, LPXLOPER v12, LPXLOPER v13, LPXLOPER v14, LPXLOPER v15, LPXLOPER v16 \
	,LPXLOPER v17, LPXLOPER v18, LPXLOPER v19) \
{ \
	return LuaFc(number, v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19); \
} 

DECLARE_EXCEL_FUNCTION(1)
DECLARE_EXCEL_FUNCTION(2)
DECLARE_EXCEL_FUNCTION(3)
DECLARE_EXCEL_FUNCTION(4)
DECLARE_EXCEL_FUNCTION(5)
DECLARE_EXCEL_FUNCTION(6)
DECLARE_EXCEL_FUNCTION(7)
DECLARE_EXCEL_FUNCTION(8)
DECLARE_EXCEL_FUNCTION(9)
DECLARE_EXCEL_FUNCTION(10)
DECLARE_EXCEL_FUNCTION(11)
DECLARE_EXCEL_FUNCTION(12)
DECLARE_EXCEL_FUNCTION(13)
DECLARE_EXCEL_FUNCTION(14)
DECLARE_EXCEL_FUNCTION(15)
DECLARE_EXCEL_FUNCTION(16)
DECLARE_EXCEL_FUNCTION(17)
DECLARE_EXCEL_FUNCTION(18)
DECLARE_EXCEL_FUNCTION(19)
DECLARE_EXCEL_FUNCTION(20)
DECLARE_EXCEL_FUNCTION(21)
DECLARE_EXCEL_FUNCTION(22)
DECLARE_EXCEL_FUNCTION(23)
DECLARE_EXCEL_FUNCTION(24)
DECLARE_EXCEL_FUNCTION(25)
DECLARE_EXCEL_FUNCTION(26)
DECLARE_EXCEL_FUNCTION(27)
DECLARE_EXCEL_FUNCTION(28)
DECLARE_EXCEL_FUNCTION(29)
DECLARE_EXCEL_FUNCTION(30)
DECLARE_EXCEL_FUNCTION(31)
DECLARE_EXCEL_FUNCTION(32)
DECLARE_EXCEL_FUNCTION(33)
DECLARE_EXCEL_FUNCTION(34)
DECLARE_EXCEL_FUNCTION(35)
DECLARE_EXCEL_FUNCTION(36)
DECLARE_EXCEL_FUNCTION(37)
DECLARE_EXCEL_FUNCTION(38)
DECLARE_EXCEL_FUNCTION(39)
DECLARE_EXCEL_FUNCTION(40)
DECLARE_EXCEL_FUNCTION(41)
DECLARE_EXCEL_FUNCTION(42)
DECLARE_EXCEL_FUNCTION(43)
DECLARE_EXCEL_FUNCTION(44)
DECLARE_EXCEL_FUNCTION(45)
DECLARE_EXCEL_FUNCTION(46)
DECLARE_EXCEL_FUNCTION(47)
DECLARE_EXCEL_FUNCTION(48)
DECLARE_EXCEL_FUNCTION(49)
DECLARE_EXCEL_FUNCTION(50)
DECLARE_EXCEL_FUNCTION(51)
DECLARE_EXCEL_FUNCTION(52)
DECLARE_EXCEL_FUNCTION(53)
DECLARE_EXCEL_FUNCTION(54)
DECLARE_EXCEL_FUNCTION(55)
DECLARE_EXCEL_FUNCTION(56)
DECLARE_EXCEL_FUNCTION(57)
DECLARE_EXCEL_FUNCTION(58)
DECLARE_EXCEL_FUNCTION(59)
DECLARE_EXCEL_FUNCTION(60)
DECLARE_EXCEL_FUNCTION(61)
DECLARE_EXCEL_FUNCTION(62)
DECLARE_EXCEL_FUNCTION(63)
DECLARE_EXCEL_FUNCTION(64)
DECLARE_EXCEL_FUNCTION(65)
DECLARE_EXCEL_FUNCTION(66)
DECLARE_EXCEL_FUNCTION(67)
DECLARE_EXCEL_FUNCTION(68)
DECLARE_EXCEL_FUNCTION(69)
DECLARE_EXCEL_FUNCTION(70)
DECLARE_EXCEL_FUNCTION(71)
DECLARE_EXCEL_FUNCTION(72)
DECLARE_EXCEL_FUNCTION(73)
DECLARE_EXCEL_FUNCTION(74)
DECLARE_EXCEL_FUNCTION(75)
DECLARE_EXCEL_FUNCTION(76)
DECLARE_EXCEL_FUNCTION(77)
DECLARE_EXCEL_FUNCTION(78)
DECLARE_EXCEL_FUNCTION(79)
DECLARE_EXCEL_FUNCTION(80)
DECLARE_EXCEL_FUNCTION(81)
DECLARE_EXCEL_FUNCTION(82)
DECLARE_EXCEL_FUNCTION(83)
DECLARE_EXCEL_FUNCTION(84)
DECLARE_EXCEL_FUNCTION(85)
DECLARE_EXCEL_FUNCTION(86)
DECLARE_EXCEL_FUNCTION(87)
DECLARE_EXCEL_FUNCTION(88)
DECLARE_EXCEL_FUNCTION(89)
DECLARE_EXCEL_FUNCTION(90)
DECLARE_EXCEL_FUNCTION(91)
DECLARE_EXCEL_FUNCTION(92)
DECLARE_EXCEL_FUNCTION(93)
DECLARE_EXCEL_FUNCTION(94)
DECLARE_EXCEL_FUNCTION(95)
DECLARE_EXCEL_FUNCTION(96)
DECLARE_EXCEL_FUNCTION(97)
DECLARE_EXCEL_FUNCTION(98)
DECLARE_EXCEL_FUNCTION(99)
DECLARE_EXCEL_FUNCTION(100)
DECLARE_EXCEL_FUNCTION(101)
DECLARE_EXCEL_FUNCTION(102)
DECLARE_EXCEL_FUNCTION(103)
DECLARE_EXCEL_FUNCTION(104)
DECLARE_EXCEL_FUNCTION(105)
DECLARE_EXCEL_FUNCTION(106)
DECLARE_EXCEL_FUNCTION(107)
DECLARE_EXCEL_FUNCTION(108)
DECLARE_EXCEL_FUNCTION(109)
DECLARE_EXCEL_FUNCTION(110)
DECLARE_EXCEL_FUNCTION(111)
DECLARE_EXCEL_FUNCTION(112)
DECLARE_EXCEL_FUNCTION(113)
DECLARE_EXCEL_FUNCTION(114)
DECLARE_EXCEL_FUNCTION(115)
DECLARE_EXCEL_FUNCTION(116)
DECLARE_EXCEL_FUNCTION(117)
DECLARE_EXCEL_FUNCTION(118)
DECLARE_EXCEL_FUNCTION(119)
DECLARE_EXCEL_FUNCTION(120)
DECLARE_EXCEL_FUNCTION(121)
DECLARE_EXCEL_FUNCTION(122)
DECLARE_EXCEL_FUNCTION(123)
DECLARE_EXCEL_FUNCTION(124)
DECLARE_EXCEL_FUNCTION(125)
DECLARE_EXCEL_FUNCTION(126)
DECLARE_EXCEL_FUNCTION(127)
DECLARE_EXCEL_FUNCTION(128)
DECLARE_EXCEL_FUNCTION(129)
DECLARE_EXCEL_FUNCTION(130)
DECLARE_EXCEL_FUNCTION(131)
DECLARE_EXCEL_FUNCTION(132)
DECLARE_EXCEL_FUNCTION(133)
DECLARE_EXCEL_FUNCTION(134)
DECLARE_EXCEL_FUNCTION(135)
DECLARE_EXCEL_FUNCTION(136)
DECLARE_EXCEL_FUNCTION(137)
DECLARE_EXCEL_FUNCTION(138)
DECLARE_EXCEL_FUNCTION(139)
DECLARE_EXCEL_FUNCTION(140)
DECLARE_EXCEL_FUNCTION(141)
DECLARE_EXCEL_FUNCTION(142)
DECLARE_EXCEL_FUNCTION(143)
DECLARE_EXCEL_FUNCTION(144)
DECLARE_EXCEL_FUNCTION(145)
DECLARE_EXCEL_FUNCTION(146)
DECLARE_EXCEL_FUNCTION(147)
DECLARE_EXCEL_FUNCTION(148)
DECLARE_EXCEL_FUNCTION(149)
DECLARE_EXCEL_FUNCTION(150)
DECLARE_EXCEL_FUNCTION(151)
DECLARE_EXCEL_FUNCTION(152)
DECLARE_EXCEL_FUNCTION(153)
DECLARE_EXCEL_FUNCTION(154)
DECLARE_EXCEL_FUNCTION(155)
DECLARE_EXCEL_FUNCTION(156)
DECLARE_EXCEL_FUNCTION(157)
DECLARE_EXCEL_FUNCTION(158)
DECLARE_EXCEL_FUNCTION(159)
DECLARE_EXCEL_FUNCTION(160)
DECLARE_EXCEL_FUNCTION(161)
DECLARE_EXCEL_FUNCTION(162)
DECLARE_EXCEL_FUNCTION(163)
DECLARE_EXCEL_FUNCTION(164)
DECLARE_EXCEL_FUNCTION(165)
DECLARE_EXCEL_FUNCTION(166)
DECLARE_EXCEL_FUNCTION(167)
DECLARE_EXCEL_FUNCTION(168)
DECLARE_EXCEL_FUNCTION(169)
DECLARE_EXCEL_FUNCTION(170)
DECLARE_EXCEL_FUNCTION(171)
DECLARE_EXCEL_FUNCTION(172)
DECLARE_EXCEL_FUNCTION(173)
DECLARE_EXCEL_FUNCTION(174)
DECLARE_EXCEL_FUNCTION(175)
DECLARE_EXCEL_FUNCTION(176)
DECLARE_EXCEL_FUNCTION(177)
DECLARE_EXCEL_FUNCTION(178)
DECLARE_EXCEL_FUNCTION(179)
DECLARE_EXCEL_FUNCTION(180)
DECLARE_EXCEL_FUNCTION(181)
DECLARE_EXCEL_FUNCTION(182)
DECLARE_EXCEL_FUNCTION(183)
DECLARE_EXCEL_FUNCTION(184)
DECLARE_EXCEL_FUNCTION(185)
DECLARE_EXCEL_FUNCTION(186)
DECLARE_EXCEL_FUNCTION(187)
DECLARE_EXCEL_FUNCTION(188)
DECLARE_EXCEL_FUNCTION(189)
DECLARE_EXCEL_FUNCTION(190)
DECLARE_EXCEL_FUNCTION(191)
DECLARE_EXCEL_FUNCTION(192)
DECLARE_EXCEL_FUNCTION(193)
DECLARE_EXCEL_FUNCTION(194)
DECLARE_EXCEL_FUNCTION(195)
DECLARE_EXCEL_FUNCTION(196)
DECLARE_EXCEL_FUNCTION(197)
DECLARE_EXCEL_FUNCTION(198)
DECLARE_EXCEL_FUNCTION(199)
DECLARE_EXCEL_FUNCTION(200)
DECLARE_EXCEL_FUNCTION(201)
DECLARE_EXCEL_FUNCTION(202)
DECLARE_EXCEL_FUNCTION(203)
DECLARE_EXCEL_FUNCTION(204)
DECLARE_EXCEL_FUNCTION(205)
DECLARE_EXCEL_FUNCTION(206)
DECLARE_EXCEL_FUNCTION(207)
DECLARE_EXCEL_FUNCTION(208)
DECLARE_EXCEL_FUNCTION(209)
DECLARE_EXCEL_FUNCTION(210)
DECLARE_EXCEL_FUNCTION(211)
DECLARE_EXCEL_FUNCTION(212)
DECLARE_EXCEL_FUNCTION(213)
DECLARE_EXCEL_FUNCTION(214)
DECLARE_EXCEL_FUNCTION(215)
DECLARE_EXCEL_FUNCTION(216)
DECLARE_EXCEL_FUNCTION(217)
DECLARE_EXCEL_FUNCTION(218)
DECLARE_EXCEL_FUNCTION(219)
DECLARE_EXCEL_FUNCTION(220)
DECLARE_EXCEL_FUNCTION(221)
DECLARE_EXCEL_FUNCTION(222)
DECLARE_EXCEL_FUNCTION(223)
DECLARE_EXCEL_FUNCTION(224)
DECLARE_EXCEL_FUNCTION(225)
DECLARE_EXCEL_FUNCTION(226)
DECLARE_EXCEL_FUNCTION(227)
DECLARE_EXCEL_FUNCTION(228)
DECLARE_EXCEL_FUNCTION(229)
DECLARE_EXCEL_FUNCTION(230)
DECLARE_EXCEL_FUNCTION(231)
DECLARE_EXCEL_FUNCTION(232)
DECLARE_EXCEL_FUNCTION(233)
DECLARE_EXCEL_FUNCTION(234)
DECLARE_EXCEL_FUNCTION(235)
DECLARE_EXCEL_FUNCTION(236)
DECLARE_EXCEL_FUNCTION(237)
DECLARE_EXCEL_FUNCTION(238)
DECLARE_EXCEL_FUNCTION(239)
DECLARE_EXCEL_FUNCTION(240)
DECLARE_EXCEL_FUNCTION(241)
DECLARE_EXCEL_FUNCTION(242)
DECLARE_EXCEL_FUNCTION(243)
DECLARE_EXCEL_FUNCTION(244)
DECLARE_EXCEL_FUNCTION(245)
DECLARE_EXCEL_FUNCTION(246)
DECLARE_EXCEL_FUNCTION(247)
DECLARE_EXCEL_FUNCTION(248)
DECLARE_EXCEL_FUNCTION(249)
DECLARE_EXCEL_FUNCTION(250)
DECLARE_EXCEL_FUNCTION(251)
DECLARE_EXCEL_FUNCTION(252)
DECLARE_EXCEL_FUNCTION(253)
DECLARE_EXCEL_FUNCTION(254)
DECLARE_EXCEL_FUNCTION(255)
DECLARE_EXCEL_FUNCTION(256)
DECLARE_EXCEL_FUNCTION(257)
DECLARE_EXCEL_FUNCTION(258)
DECLARE_EXCEL_FUNCTION(259)
DECLARE_EXCEL_FUNCTION(260)
DECLARE_EXCEL_FUNCTION(261)
DECLARE_EXCEL_FUNCTION(262)
DECLARE_EXCEL_FUNCTION(263)
DECLARE_EXCEL_FUNCTION(264)
DECLARE_EXCEL_FUNCTION(265)
DECLARE_EXCEL_FUNCTION(266)
DECLARE_EXCEL_FUNCTION(267)
DECLARE_EXCEL_FUNCTION(268)
DECLARE_EXCEL_FUNCTION(269)
DECLARE_EXCEL_FUNCTION(270)
DECLARE_EXCEL_FUNCTION(271)
DECLARE_EXCEL_FUNCTION(272)
DECLARE_EXCEL_FUNCTION(273)
DECLARE_EXCEL_FUNCTION(274)
DECLARE_EXCEL_FUNCTION(275)
DECLARE_EXCEL_FUNCTION(276)
DECLARE_EXCEL_FUNCTION(277)
DECLARE_EXCEL_FUNCTION(278)
DECLARE_EXCEL_FUNCTION(279)
DECLARE_EXCEL_FUNCTION(280)
DECLARE_EXCEL_FUNCTION(281)
DECLARE_EXCEL_FUNCTION(282)
DECLARE_EXCEL_FUNCTION(283)
DECLARE_EXCEL_FUNCTION(284)
DECLARE_EXCEL_FUNCTION(285)
DECLARE_EXCEL_FUNCTION(286)
DECLARE_EXCEL_FUNCTION(287)
DECLARE_EXCEL_FUNCTION(288)
DECLARE_EXCEL_FUNCTION(289)
DECLARE_EXCEL_FUNCTION(290)
DECLARE_EXCEL_FUNCTION(291)
DECLARE_EXCEL_FUNCTION(292)
DECLARE_EXCEL_FUNCTION(293)
DECLARE_EXCEL_FUNCTION(294)
DECLARE_EXCEL_FUNCTION(295)
DECLARE_EXCEL_FUNCTION(296)
DECLARE_EXCEL_FUNCTION(297)
DECLARE_EXCEL_FUNCTION(298)
DECLARE_EXCEL_FUNCTION(299)
DECLARE_EXCEL_FUNCTION(300)
DECLARE_EXCEL_FUNCTION(301)
DECLARE_EXCEL_FUNCTION(302)
DECLARE_EXCEL_FUNCTION(303)
DECLARE_EXCEL_FUNCTION(304)
DECLARE_EXCEL_FUNCTION(305)
DECLARE_EXCEL_FUNCTION(306)
DECLARE_EXCEL_FUNCTION(307)
DECLARE_EXCEL_FUNCTION(308)
DECLARE_EXCEL_FUNCTION(309)
DECLARE_EXCEL_FUNCTION(310)
DECLARE_EXCEL_FUNCTION(311)
DECLARE_EXCEL_FUNCTION(312)
DECLARE_EXCEL_FUNCTION(313)
DECLARE_EXCEL_FUNCTION(314)
DECLARE_EXCEL_FUNCTION(315)
DECLARE_EXCEL_FUNCTION(316)
DECLARE_EXCEL_FUNCTION(317)
DECLARE_EXCEL_FUNCTION(318)
DECLARE_EXCEL_FUNCTION(319)
DECLARE_EXCEL_FUNCTION(320)
DECLARE_EXCEL_FUNCTION(321)
DECLARE_EXCEL_FUNCTION(322)
DECLARE_EXCEL_FUNCTION(323)
DECLARE_EXCEL_FUNCTION(324)
DECLARE_EXCEL_FUNCTION(325)
DECLARE_EXCEL_FUNCTION(326)
DECLARE_EXCEL_FUNCTION(327)
DECLARE_EXCEL_FUNCTION(328)
DECLARE_EXCEL_FUNCTION(329)
DECLARE_EXCEL_FUNCTION(330)
DECLARE_EXCEL_FUNCTION(331)
DECLARE_EXCEL_FUNCTION(332)
DECLARE_EXCEL_FUNCTION(333)
DECLARE_EXCEL_FUNCTION(334)
DECLARE_EXCEL_FUNCTION(335)
DECLARE_EXCEL_FUNCTION(336)
DECLARE_EXCEL_FUNCTION(337)
DECLARE_EXCEL_FUNCTION(338)
DECLARE_EXCEL_FUNCTION(339)
DECLARE_EXCEL_FUNCTION(340)
DECLARE_EXCEL_FUNCTION(341)
DECLARE_EXCEL_FUNCTION(342)
DECLARE_EXCEL_FUNCTION(343)
DECLARE_EXCEL_FUNCTION(344)
DECLARE_EXCEL_FUNCTION(345)
DECLARE_EXCEL_FUNCTION(346)
DECLARE_EXCEL_FUNCTION(347)
DECLARE_EXCEL_FUNCTION(348)
DECLARE_EXCEL_FUNCTION(349)
DECLARE_EXCEL_FUNCTION(350)
DECLARE_EXCEL_FUNCTION(351)
DECLARE_EXCEL_FUNCTION(352)
DECLARE_EXCEL_FUNCTION(353)
DECLARE_EXCEL_FUNCTION(354)
DECLARE_EXCEL_FUNCTION(355)
DECLARE_EXCEL_FUNCTION(356)
DECLARE_EXCEL_FUNCTION(357)
DECLARE_EXCEL_FUNCTION(358)
DECLARE_EXCEL_FUNCTION(359)
DECLARE_EXCEL_FUNCTION(360)
DECLARE_EXCEL_FUNCTION(361)
DECLARE_EXCEL_FUNCTION(362)
DECLARE_EXCEL_FUNCTION(363)
DECLARE_EXCEL_FUNCTION(364)
DECLARE_EXCEL_FUNCTION(365)
DECLARE_EXCEL_FUNCTION(366)
DECLARE_EXCEL_FUNCTION(367)
DECLARE_EXCEL_FUNCTION(368)
DECLARE_EXCEL_FUNCTION(369)
DECLARE_EXCEL_FUNCTION(370)
DECLARE_EXCEL_FUNCTION(371)
DECLARE_EXCEL_FUNCTION(372)
DECLARE_EXCEL_FUNCTION(373)
DECLARE_EXCEL_FUNCTION(374)
DECLARE_EXCEL_FUNCTION(375)
DECLARE_EXCEL_FUNCTION(376)
DECLARE_EXCEL_FUNCTION(377)
DECLARE_EXCEL_FUNCTION(378)
DECLARE_EXCEL_FUNCTION(379)
DECLARE_EXCEL_FUNCTION(380)
DECLARE_EXCEL_FUNCTION(381)
DECLARE_EXCEL_FUNCTION(382)
DECLARE_EXCEL_FUNCTION(383)
DECLARE_EXCEL_FUNCTION(384)
DECLARE_EXCEL_FUNCTION(385)
DECLARE_EXCEL_FUNCTION(386)
DECLARE_EXCEL_FUNCTION(387)
DECLARE_EXCEL_FUNCTION(388)
DECLARE_EXCEL_FUNCTION(389)
DECLARE_EXCEL_FUNCTION(390)
DECLARE_EXCEL_FUNCTION(391)
DECLARE_EXCEL_FUNCTION(392)
DECLARE_EXCEL_FUNCTION(393)
DECLARE_EXCEL_FUNCTION(394)
DECLARE_EXCEL_FUNCTION(395)
DECLARE_EXCEL_FUNCTION(396)
DECLARE_EXCEL_FUNCTION(397)
DECLARE_EXCEL_FUNCTION(398)
DECLARE_EXCEL_FUNCTION(399)
DECLARE_EXCEL_FUNCTION(400)
DECLARE_EXCEL_FUNCTION(401)
DECLARE_EXCEL_FUNCTION(402)
DECLARE_EXCEL_FUNCTION(403)
DECLARE_EXCEL_FUNCTION(404)
DECLARE_EXCEL_FUNCTION(405)
DECLARE_EXCEL_FUNCTION(406)
DECLARE_EXCEL_FUNCTION(407)
DECLARE_EXCEL_FUNCTION(408)
DECLARE_EXCEL_FUNCTION(409)
DECLARE_EXCEL_FUNCTION(410)
DECLARE_EXCEL_FUNCTION(411)
DECLARE_EXCEL_FUNCTION(412)
DECLARE_EXCEL_FUNCTION(413)
DECLARE_EXCEL_FUNCTION(414)
DECLARE_EXCEL_FUNCTION(415)
DECLARE_EXCEL_FUNCTION(416)
DECLARE_EXCEL_FUNCTION(417)
DECLARE_EXCEL_FUNCTION(418)
DECLARE_EXCEL_FUNCTION(419)
DECLARE_EXCEL_FUNCTION(420)
DECLARE_EXCEL_FUNCTION(421)
DECLARE_EXCEL_FUNCTION(422)
DECLARE_EXCEL_FUNCTION(423)
DECLARE_EXCEL_FUNCTION(424)
DECLARE_EXCEL_FUNCTION(425)
DECLARE_EXCEL_FUNCTION(426)
DECLARE_EXCEL_FUNCTION(427)
DECLARE_EXCEL_FUNCTION(428)
DECLARE_EXCEL_FUNCTION(429)
DECLARE_EXCEL_FUNCTION(430)
DECLARE_EXCEL_FUNCTION(431)
DECLARE_EXCEL_FUNCTION(432)
DECLARE_EXCEL_FUNCTION(433)
DECLARE_EXCEL_FUNCTION(434)
DECLARE_EXCEL_FUNCTION(435)
DECLARE_EXCEL_FUNCTION(436)
DECLARE_EXCEL_FUNCTION(437)
DECLARE_EXCEL_FUNCTION(438)
DECLARE_EXCEL_FUNCTION(439)
DECLARE_EXCEL_FUNCTION(440)
DECLARE_EXCEL_FUNCTION(441)
DECLARE_EXCEL_FUNCTION(442)
DECLARE_EXCEL_FUNCTION(443)
DECLARE_EXCEL_FUNCTION(444)
DECLARE_EXCEL_FUNCTION(445)
DECLARE_EXCEL_FUNCTION(446)
DECLARE_EXCEL_FUNCTION(447)
DECLARE_EXCEL_FUNCTION(448)
DECLARE_EXCEL_FUNCTION(449)
DECLARE_EXCEL_FUNCTION(450)
DECLARE_EXCEL_FUNCTION(451)
DECLARE_EXCEL_FUNCTION(452)
DECLARE_EXCEL_FUNCTION(453)
DECLARE_EXCEL_FUNCTION(454)
DECLARE_EXCEL_FUNCTION(455)
DECLARE_EXCEL_FUNCTION(456)
DECLARE_EXCEL_FUNCTION(457)
DECLARE_EXCEL_FUNCTION(458)
DECLARE_EXCEL_FUNCTION(459)
DECLARE_EXCEL_FUNCTION(460)
DECLARE_EXCEL_FUNCTION(461)
DECLARE_EXCEL_FUNCTION(462)
DECLARE_EXCEL_FUNCTION(463)
DECLARE_EXCEL_FUNCTION(464)
DECLARE_EXCEL_FUNCTION(465)
DECLARE_EXCEL_FUNCTION(466)
DECLARE_EXCEL_FUNCTION(467)
DECLARE_EXCEL_FUNCTION(468)
DECLARE_EXCEL_FUNCTION(469)
DECLARE_EXCEL_FUNCTION(470)
DECLARE_EXCEL_FUNCTION(471)
DECLARE_EXCEL_FUNCTION(472)
DECLARE_EXCEL_FUNCTION(473)
DECLARE_EXCEL_FUNCTION(474)
DECLARE_EXCEL_FUNCTION(475)
DECLARE_EXCEL_FUNCTION(476)
DECLARE_EXCEL_FUNCTION(477)
DECLARE_EXCEL_FUNCTION(478)
DECLARE_EXCEL_FUNCTION(479)
DECLARE_EXCEL_FUNCTION(480)
DECLARE_EXCEL_FUNCTION(481)
DECLARE_EXCEL_FUNCTION(482)
DECLARE_EXCEL_FUNCTION(483)
DECLARE_EXCEL_FUNCTION(484)
DECLARE_EXCEL_FUNCTION(485)
DECLARE_EXCEL_FUNCTION(486)
DECLARE_EXCEL_FUNCTION(487)
DECLARE_EXCEL_FUNCTION(488)
DECLARE_EXCEL_FUNCTION(489)
DECLARE_EXCEL_FUNCTION(490)
DECLARE_EXCEL_FUNCTION(491)
DECLARE_EXCEL_FUNCTION(492)
DECLARE_EXCEL_FUNCTION(493)
DECLARE_EXCEL_FUNCTION(494)
DECLARE_EXCEL_FUNCTION(495)
DECLARE_EXCEL_FUNCTION(496)
DECLARE_EXCEL_FUNCTION(497)
DECLARE_EXCEL_FUNCTION(498)
DECLARE_EXCEL_FUNCTION(499)
DECLARE_EXCEL_FUNCTION(500)
DECLARE_EXCEL_FUNCTION(501)
DECLARE_EXCEL_FUNCTION(502)
DECLARE_EXCEL_FUNCTION(503)
DECLARE_EXCEL_FUNCTION(504)
DECLARE_EXCEL_FUNCTION(505)
DECLARE_EXCEL_FUNCTION(506)
DECLARE_EXCEL_FUNCTION(507)
DECLARE_EXCEL_FUNCTION(508)
DECLARE_EXCEL_FUNCTION(509)
DECLARE_EXCEL_FUNCTION(510)
DECLARE_EXCEL_FUNCTION(511)

