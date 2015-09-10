#include "hal.h"
#include "device/video.h"
#include "device/palette.h"

#include <string.h>
#include <stdlib.h>

int get_fps();

/*

    [SCREEN]

  *---> col, x, left, right, width
  |
  |
  v row, y, top, bottom, height

*/

#define BYTE_PIXEL_OFFSET(dst, x, y) (((dst)->pitch) * (y) + (x))
#define BYTE_PIXEL_PTR(dst, x, y) ({ \
    SDL_Surface *__dst = (dst); \
    assert(__dst->pitch == __dst->w); \
    (((char *) (__dst->pixels)) + BYTE_PIXEL_OFFSET(__dst, x, y)); \
})

#define get_left(rect) ((rect)->x)
#define get_top(rect) ((rect)->y)
#define get_right(rect) (get_left(rect) + (rect)->w) // macro not safe
#define get_bottom(rect) (get_top(rect) + (rect)->h) // macro not safe

#define max(a, b) ((a) > (b) ? (a) : (b)) // macro not safe
#define min(a, b) ((a) < (b) ? (a) : (b)) // macro not safe

typedef int SDL_bool;
#define SDL_TRUE (1)
#define SDL_FALSE (0)
SDL_bool SDL_IntersectRect(const SDL_Rect* A, const SDL_Rect* B, SDL_Rect* result)
{
    int left, right, top, bottom;
    left = max(get_left(A), get_left(B));
    top = max(get_top(A), get_top(B));
    right = min(get_right(A), get_right(B));
    bottom = min(get_bottom(A), get_bottom(B));
    
    if (right > left && bottom > top) {
        result->x = left;
        result->y = top;
        result->w = right - left;
        result->h = bottom - top;
        return SDL_TRUE;
    } else {
        return SDL_FALSE;
    }
}


void GetSurfaceRect(SDL_Surface *surface, SDL_Rect *rect)
{
    rect->x = 0;
    rect->y = 0;
    rect->w = surface->w;
    rect->h = surface->h;
}

void SDL_GetClipRect(SDL_Surface *surface, SDL_Rect *rect)
{
    //printf("x=%d y=%d ", (int) surface->clip_rect.x, (int) surface->clip_rect.y);
    //printf("h=%d w=%d\n", (unsigned) surface->clip_rect.h, (unsigned) surface->clip_rect.w);
    SDL_Rect *clip_rect = &surface->clip_rect;
    if (clip_rect->x == 0 && clip_rect->y == 0 && clip_rect->h == 0 && clip_rect->w == 0) {
        GetSurfaceRect(surface, rect);
    } else {
        *rect = *clip_rect;
    }
    
    assert(rect->x >= 0 && rect->y >= 0);
    assert(rect->x + rect->w <= surface->w);
    assert(rect->y + rect->h <= surface->h);
}

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(dst && src);

	/* TODO: Performs a fast blit from the source surface to the 
	 * destination surface. Only the position is used in the
	 * ``dstrect'' (the width and height are ignored). If either
	 * ``srcrect'' or ``dstrect'' are NULL, the entire surface 
	 * (``src'' or ``dst'') is copied. The final blit rectangle 
	 * is saved in ``dstrect'' after all clipping is performed
	 * (``srcrect'' is not modified).
	 */
	
	SDL_bool bret;
	
	// get src surface rect;
	SDL_Rect src_surface_rect;
	GetSurfaceRect(src, &src_surface_rect);
	if (srcrect) {
    	bret = SDL_IntersectRect(srcrect, &src_surface_rect, &src_surface_rect);
        if (bret == SDL_FALSE) return;
    }
	
	// get dst surface rect
    SDL_Rect dst_surface_rect;
    SDL_GetClipRect(dst, &dst_surface_rect);
	if (dstrect) {
	    SDL_Rect trect;
    	trect.x = dstrect->x;
	    trect.y = dstrect->y;
	    trect.w = src_surface_rect.w;
	    trect.h = src_surface_rect.h;
	    bret = SDL_IntersectRect(&trect, &dst_surface_rect, &dst_surface_rect);
        if (bret == SDL_FALSE) return;
	}

	
	// calc height and width needed to copy
	int copy_width = min(src_surface_rect.w, dst_surface_rect.w);
	int copy_height = min(src_surface_rect.h, dst_surface_rect.h);
	
	// do real copy
	int dst_line, src_line;
	for (dst_line = get_top(&dst_surface_rect),
	     src_line = get_top(&src_surface_rect);
	     
	        copy_height--;
	        
	            dst_line++,
	            src_line++
	    ) {
	    memcpy(BYTE_PIXEL_PTR(dst, dst_surface_rect.x, dst_line), 
	           BYTE_PIXEL_PTR(src, src_surface_rect.x, src_line),
	           copy_width);
	    assert(dst_line < dst->h);
	    assert(src_line < src->h);
	    assert(dst_surface_rect.x + copy_width <= dst->w);
	    assert(src_surface_rect.x + copy_width <= src->w);
	}
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
	assert(dst);
	assert(color <= 0xff);

	/* DONE: Fill the rectangle area described by ``dstrect''
	 * in surface ``dst'' with color ``color''. If dstrect is
	 * NULL, fill the whole surface.
	 */
	
	SDL_bool bret;
	
	// get dst surface rect
    SDL_Rect dst_surface_rect;
    SDL_GetClipRect(dst, &dst_surface_rect);
    if (dstrect) {
	    bret = SDL_IntersectRect(dstrect, &dst_surface_rect, &dst_surface_rect);
        if (bret == SDL_FALSE) return;
	}
	
	int fill_height, fill_width;
	fill_width = dst_surface_rect.w;
	fill_height = dst_surface_rect.h;
	
	// do real memset
	int dst_line;
	for (dst_line = get_top(&dst_surface_rect); fill_height--; dst_line++) {
	    memset(BYTE_PIXEL_PTR(dst, dst_surface_rect.x, dst_line), color, fill_width);
	    assert(dst_line < dst->h);
	    assert(dst_surface_rect.x + fill_width <= dst->w);
	}
}

void SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h) {
	assert(screen);
	assert(screen->pitch == 320);
	if(screen->flags & SDL_HWSURFACE) {
		// update entire video memory
		ZBY_TIMING_BEGIN(0);
        memcpy((void *) VMEM_ADDR, screen->pixels, screen->pitch * screen->h);
        //ZBY_TIMING_END(0, "copy to video memory");
		incr_nr_draw();
		
		if(x == 0 && y == 0) {
			/* Draw FPS */
			vmem = VMEM_ADDR;
			char buf[80];
			sprintf(buf, "%dFPS", get_fps());
			draw_string(buf, 0, 0, 10);
		}
		
		return;
	}

    /* Copy the pixels in the rectangle area to the screen. */
    // ZBY: this is no need to do this
    //      since we directly write to video memory
	assert(0);
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, 
		int firstcolor, int ncolors) {
	assert(s);
	assert(s->format);
	assert(s->format->palette);
	assert(firstcolor == 0);

	if(s->format->palette->colors == NULL || s->format->palette->ncolors != ncolors) {
		if(s->format->palette->ncolors != ncolors && s->format->palette->colors != NULL) {
			/* If the size of the new palette is different 
			 * from the old one, free the old one.
			 */
			free(s->format->palette->colors);
		}

		/* Get new memory space to store the new palette. */
		s->format->palette->colors = malloc(sizeof(SDL_Color) * ncolors);
		assert(s->format->palette->colors);
	}

	/* Set the new palette. */
	s->format->palette->ncolors = ncolors;
	memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

	if(s->flags & SDL_HWSURFACE) {
		/* DONE: Set the VGA palette by calling write_palette(). */
		write_palette(colors, ncolors);
	}
}

/* ======== The following functions are already implemented. ======== */

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *scrrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(src && dst);
	int x = (scrrect == NULL ? 0 : scrrect->x);
	int y = (scrrect == NULL ? 0 : scrrect->y);
	int w = (scrrect == NULL ? src->w : scrrect->w);
	int h = (scrrect == NULL ? src->h : scrrect->h);

	assert(dstrect);
	if(w == dstrect->w && h == dstrect->h) {
		/* The source rectangle and the destination rectangle
		 * are of the same size. If that is the case, there
		 * is no need to stretch, just copy. */
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_BlitSurface(src, &rect, dst, dstrect);
	}
	else {
		/* No other case occurs in NEMU-PAL. */
		assert(0);
	}
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
		uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
	SDL_Surface *s = malloc(sizeof(SDL_Surface));
	assert(s);
	s->format = malloc(sizeof(SDL_PixelFormat));
	assert(s);
	s->format->palette = malloc(sizeof(SDL_Palette));
	assert(s->format->palette);
	s->format->palette->colors = NULL;

	s->format->BitsPerPixel = depth;

	s->flags = flags;
	s->w = width;
	s->h = height;
	
	/* ZBY: init clip_rect */
	s->clip_rect.x = 0;
	s->clip_rect.y = 0;
	s->clip_rect.w = s->w;
	s->clip_rect.h = s->h;
	
	s->pitch = (width * depth) >> 3;
	// ZBY: alloc buffer for SDL_HWSURFACE since directly write to VMEM is slow
	//s->pixels = (flags & SDL_HWSURFACE ? (void *)VMEM_ADDR : malloc(s->pitch * height));
	s->pixels = malloc(s->pitch * height);
	assert(s->pixels);

	return s;
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
	return SDL_CreateRGBSurface(flags,  width, height, bpp,
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void SDL_FreeSurface(SDL_Surface *s) {
	if(s != NULL) {
		if(s->format != NULL) {
			if(s->format->palette != NULL) {
				if(s->format->palette->colors != NULL) {
					free(s->format->palette->colors);
				}
				free(s->format->palette);
			}

			free(s->format);
		}
		
		if(s->pixels != NULL) {
			free(s->pixels);
		}

		free(s);
	}
}

