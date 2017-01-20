
#ifndef POINT_SEARCH_EXPORT_H_INCLUDED
#define POINT_SEARCH_EXPORT_H_INCLUDED

extern "C" 
{

__declspec(dllexport) SearchContext*  __stdcall create(const Point* points_begin, const Point* points_end);

__declspec(dllexport) int32_t __stdcall search(SearchContext* sc, const Rect rect, const int32_t count, Point* out_points);

__declspec(dllexport) SearchContext* __stdcall destroy(SearchContext* sc);

};

#endif 
