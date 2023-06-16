APP_OUT=app_event

default:
	$(CROSS_COMPILE)gcc main.c -o $(APP_OUT).out -lpthread
	$(CROSS_COMPILE)gcc touch.c -o touch_$(APP_OUT).out -lpthread
