#include <stdio.h>
#include <SDL3/SDL.h>
#include <math.h>

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define RAYS_NUMBER 100
#define COLOR_GRAY 0xFFD736

struct Circle{
	double x;
	double y;
	double radius;
};

struct Ray{
	double x_start;
	double y_start;
	double angle;
};

void FillCircle(SDL_Renderer* renderer,struct Circle circle){
	double radius_squared=pow(circle.radius,2); 
	for(double x=circle.x-circle.radius;x<=circle.x+circle.radius;x++){
		for(double y=circle.y-circle.radius;y<=circle.y+circle.radius;y++){
			double distance_squared=pow(x-circle.x,2)+pow(y-circle.y,2);
			if(distance_squared<=radius_squared){
				SDL_RenderPoint(renderer,x,y);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
		}
	}
}

void generate_rays(struct Circle circle,struct Ray  rays[RAYS_NUMBER]){
	
	for(int i=0;i<RAYS_NUMBER;i++){
		double angle=((double)i/RAYS_NUMBER)*2*M_PI;
		struct Ray ray={circle.x,circle.y,angle};
		rays[i]=ray;
		printf("Angle: %f\n",angle);
	}
}

void FillRays(SDL_Renderer* renderer,struct Ray rays[RAYS_NUMBER],struct Circle object){
	double radius_squared=pow(object.radius,2);

	for(int i=0;i<RAYS_NUMBER;i++){
		struct Ray ray=rays[i];
		
		int end_of_screen=0;
		int object_hit=0;
		
		double step=1;
		double x_draw=ray.x_start;
		double y_draw=ray.y_start;
		while(!end_of_screen && !object_hit){
			
			x_draw=x_draw+step*cos(ray.angle);
			y_draw=y_draw+step*sin(ray.angle);

			SDL_SetRenderDrawColor(renderer, 255, 215, 54, 255);
			SDL_RenderPoint(renderer,x_draw,y_draw);
			
			if(x_draw<0 || x_draw>WIDTH){
				end_of_screen=1;
			}
			if(y_draw<0 || y_draw>HEIGHT){
				end_of_screen=1; 
			}
			double distance_squared=pow(x_draw-object.x,2)+pow(y_draw-object.y,2);
			if(distance_squared<radius_squared){
				break;
			}
		}
	}
}

int main(){
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window=SDL_CreateWindow("Raytracing",WIDTH,HEIGHT,0);

	SDL_Renderer* renderer=SDL_CreateRenderer(window,NULL);
	// SDL_Rect rect=(SDL_Rect){200,200,200,200};
	// SDL_FillSurfaceRect(surface,&rect,COLOR_WHITE);

	//SDL_FillSurfaceRect(surface, NULL, 0xff000000);

	struct Circle circle={200,200,40};
	struct Circle shadow_circle={550,300,140};

	struct Ray rays[RAYS_NUMBER];
	generate_rays(circle,rays);

	double obstacle_speed_y=1;
	int simulation_running=1;	
	SDL_Event event;

	SDL_FRect erase_rect={0,0,(float)WIDTH,(float)HEIGHT};

	while(simulation_running){
		while(SDL_PollEvent(&event)){
			if(event.type==SDL_EVENT_QUIT){
				simulation_running=0;
			}
			if(event.type==SDL_EVENT_MOUSE_MOTION && event.motion.state!=0){
				circle.x=event.motion.x;
				circle.y=event.motion.y;
				generate_rays(circle,rays);

			}
		}

		SDL_SetRenderDrawColor(renderer,0,0,0,0);
		SDL_RenderFillRect(renderer,&erase_rect);

		SDL_SetRenderDrawColor(renderer,255,255,255,255);

		FillRays(renderer,rays,shadow_circle);
		FillCircle(renderer,circle);
		FillCircle(renderer,shadow_circle);

		shadow_circle.y+=obstacle_speed_y;
		if(shadow_circle.y-shadow_circle.radius<0){
			obstacle_speed_y=-obstacle_speed_y;
		}
		if(shadow_circle.y+shadow_circle.radius>HEIGHT){
			obstacle_speed_y=-obstacle_speed_y;
		}

		SDL_RenderPresent(renderer);

		SDL_Delay(10);
	}
}