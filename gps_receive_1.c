#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

#define baudrate B9600
#define MODEMDEVICE "/dev/ttySAC2"
#define _POSIX_SOURCE 1

void receive_gps_info(char *buf);
void set_uart_conifg();


void set_uart_conifg(){
	int fd;
	int read_status;
	char read_buf[512];
	struct termios new_cfg, old_cfg;
	
	fd = open(MODEMDEVICE, O_RDWR, O_NOCTTY );//打开串口
	if(fd < 0){
		perror("open uart_com ");
		exit(-1);
	}
	else{
		printf("open the uart successfuly\n");
	}
	tcgetattr(fd,&old_cfg);
	memset(&new_cfg, 0, sizeof(new_cfg));
	
	new_cfg.c_cflag |= CLOCAL | CREAD | HUPCL;//本地连接和接受使能
	cfmakeraw(&new_cfg);//设置串口的工作模式为串口
	cfsetispeed(&new_cfg,baudrate);//设置输入波特率
	cfsetospeed(&new_cfg,baudrate);//设置输出波特率
	new_cfg.c_cflag &= ~CSIZE;//清除原理的数据设置
	new_cfg.c_cflag |= CS8;//8位数据
	new_cfg.c_cflag &= ~PARENB;//没有奇偶校验位
	new_cfg.c_cflag &= ~CSTOPB;//一位停止位
	new_cfg.c_iflag = IGNBRK;
	new_cfg.c_oflag = 0;
	new_cfg.c_lflag = ICANON; 
	
	tcflush(fd, TCIFLUSH);// 清除缓冲中尚未处理的数据
	if(tcsetattr(fd, TCSANOW, &new_cfg) != 0){
		perror("tcsetattr");
		exit(-1);
	}	
	while(1){
		printf("\nreceive the information about the GPS:");
		fflush(stdout);//标准输出缓冲区
		
		read_status = read(fd, read_buf, 255);//从串口中度
		if(read_status == -1){
			perror("read error");
			exit(-1);
		}
		printf("%s\n", read_buf);
		receive_gps_info(read_buf);
		memset(read_buf, 0, sizeof(read_buf));	
	}
	tcsetattr(fd, TCSANOW, &old_cfg );
}
///////////////////////////////////////////////////////////////////////////////////
void receive_gps_info(char *buf){
		
	int len, gps_i = 0, gps_ii = 0, count = 0, count_1 = 0;
	char *str_point, *str_status;//定义gps数据移动指针
	char GPS_DATA[512], command[] = "$GPGGA";//存放数据

	int time[20];//时间
	char longitude[12];//经度
	char earth_E_W;//半球
	char latitude[12];//纬度
	char earth_N_S;//半球
	int satellite[2];//卫星数
	char precision[4];//水平精度
	char hight[8];//离海平面的水平高度
	char right;//精度
	char danwei ;//单位

	printf("%s\n", buf);
	strcpy(GPS_DATA, buf);
	memset(buf, 0, sizeof(buf));
	
	len = strlen(GPS_DATA);
	if(len < 30 && len > 75){
		printf("GPS_DATA is error !\n");
		exit(-1);
	}
	//////////////////进行$GPSGGA///////////////对比///////////////////
	if(GPS_DATA[0] == '$' && GPS_DATA[1] == 'G' && GPS_DATA[2] == 'P' && 
		GPS_DATA[3] == 'G' && GPS_DATA[4] == 'G' && GPS_DATA[5] == 'A'){
		
		str_status = strstr(GPS_DATA, ",");//获得第一个逗号位置
		///////////////////time//////////////////////
		str_status ++;
		str_point = strstr(str_status, ",");//第二个逗号位置
		//str_point = str_status;
		while(str_status != str_point){
			time[gps_i] = *str_status - '0';//获取数据
			gps_i ++;
			str_status ++;
		}
		gps_i = 0;
		///////////////////longitude///////////////////
		str_point ++ ;
		str_status = strstr(str_point, ",");//第三个逗号位置
		while(str_point != str_status){
			if(*str_point == '.'){
				longitude[gps_i] = *str_point;
				gps_i ++ ;
				str_point ++;	
			}
			longitude[gps_i] = *str_point ;
			gps_i ++;
			str_point ++;
				
		}
		gps_i = 0;
		///////////////////N-S//////////////////////
		str_status ++;
		earth_N_S = *str_status;	
		str_status ++;	//第四个逗号位置
		///////////////////latitude/////////////////
		str_status ++;
		str_point = strstr(str_status, ",");//第五个逗号位置
		while(str_status != str_point){
			if(*str_status == '.'){
				latitude[gps_i] = *str_status;
				gps_i ++ ;
				str_status ++;	
			}
			latitude[gps_i] = *str_status;
			gps_i ++;
			str_status ++;
		}
		gps_i = 0;
		////////////////E--W//////////////////
		str_point ++;
		earth_E_W = *str_point;
		str_point ++;//第六个逗号
		////////////////ritght////////////////
		str_point ++;
		right = *str_point;
		str_point ++;//第七个逗号位置
		///////////////satellite//////////////
		str_point ++;
		str_status = strstr(str_point, ",");//第八个逗号位置
		while(str_point != str_status){
			satellite[gps_i] = *str_point - '0';
			gps_i ++;
			str_point ++;
		}
		gps_i = 0;
		//////////////////precision/////精度////////
		str_status ++;
		str_point = strstr(str_status, ",");//第九个逗号位置
			while(str_status != str_point){
				if(*str_status == '.'){
					precision[gps_i] = *str_status;
					gps_i ++;
					str_status ++;
				}
				precision[gps_i] = *str_status;
				gps_i ++;
				str_status ++;
				count ++;
			}
		gps_i = 0;
		str_point ++;
		//////////////////hight//////海拔高度//////////////
		str_status = strstr(str_point, ",");//第十个逗号位置
		while(str_point != str_status){
			if(*str_point == '-' ){
				hight[gps_i] = *str_point;
				gps_i ++;
				str_point ++;
			}
			if(*str_point == '.'){
				hight[gps_i] = *str_point;
				gps_i ++;
				str_point ++;
			}
			hight[gps_i] = *str_point;
			gps_i ++;
			str_point ++;
			count_1 ++;
		}
		str_status ++;
		danwei = *str_status;
	
		/////////////////////显示//////////////////////////////
		//////////////////////time/////////////////
		printf("\n");
		printf("TIME ");
		for(gps_ii = 0; gps_ii < 2; gps_ii ++){
			printf("%d",time[gps_ii]);
		}
		printf(":");
		for(gps_ii = 2; gps_ii < 4; gps_ii ++){
			printf("%d",time[gps_ii]);
		}
		printf(":");
		for(gps_ii = 4; gps_ii < 6; gps_ii ++){
			printf("%d",time[gps_ii]);
		}
		/////////////////////longitude//////////////
		printf(" LONGTIUDE:");
		printf("%s",longitude);
		
		printf("%c  ",earth_N_S);
		/////////////////////latitude//////////////
		printf(" LATITUDE:");	
		printf("%s",latitude);

		printf("%c  ",earth_E_W);
		///////////////////right///////////////////
		printf(" RIGHT:");
		
		printf("%c",right);
		////////////////////satellite///////////////
		printf(" SATELLITE:");
		for(gps_ii = 0; gps_ii < 2; gps_ii ++){
			printf("%d",satellite[gps_ii]);
		}
		////////////////////precision/////////////////
		printf(" PRECISION:");
		printf("%s",precision);
		/////////////////////hight////////////////////
		printf(" HIGHT:");
		printf("%s",hight);
	
		printf("%c",danwei);
	
	}
	else{
		printf("please waiting.......... \n");
	}	
}	
int main(){
	set_uart_conifg();

return 0;	
}

