#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

#define baudrate B9600
#define MODEMDEVICE "/dev/ttySAC2"
#define _POSIX_SOURCE 1


void set_uart_conifg(){
	int fd;
	int read_status, len, gps_i = 0, count, gps_ii = 0;
	char *str_point, *str_status, *str_status_1, *str_status_2, *str_status_3, *str_status_4;
	char read_buf[512], GPS_DATA[512], command[] = "$GPGGA";
	struct termios new_cfg, old_cfg;

	int time[20];//时间
	int longitude[12];//经度
	char earth_E_W;//半球
	int latitude[12];//纬度
	char earth_N_S;//半球
	int satellite[2];//卫星数
	int precision[4];//水平精度
	int hight[8];//离海平面的水平高度
	int right;
	//char point = 96;

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
		printf("receive the information about the GPS:");
		fflush(stdout);//标准输出缓冲区
		
		read_status = read(fd, read_buf, 255);//从串口中度
		if(read_status == -1){
			perror("read error");
			exit(-1);
		}
		printf("%s\n", read_buf);
		strcpy(GPS_DATA, read_buf);
		memset(read_buf, 0, sizeof(read_buf));
	
		len = strlen(GPS_DATA);
		if(len < 30 && len > 75){
			printf("GPS_DATA is error !\n");
			exit(-1);
		}
	//	str_status = strstr(GPS_DATA, command);//判断是不是"$GPGGA"
	//	printf("%c \n", *str_status);
		if(GPS_DATA[0] == '$' && GPS_DATA[1] == 'G' && GPS_DATA[2] == 'P' && 
			GPS_DATA[3] == 'G' && GPS_DATA[4] == 'G' && GPS_DATA[5] == 'A'){
		
			str_status = strstr(GPS_DATA, ",");
			///////////////////time//////////////////////
			str_status ++;
			str_point = strstr(str_status, ",");
			//str_point = str_status;
			while(str_status != str_point){
				time[gps_i] = *str_status - '0';
				gps_i ++;
			 	str_status ++;
			}
			gps_i = 0;
			///////////////////longitude///////////////////
			str_point ++ ;
			str_status = strstr(str_point, ",");
			while(str_point != str_status){
				if(*str_point == '.'){
					longitude[gps_i] = '.';
					gps_i ++ ;
					str_point ++;	
				}
				longitude[gps_i] = *str_point - '0';
				gps_i ++;
				str_point ++;
				
			}
			gps_i = 0;
			///////////////////N-S//////////////////////
			str_status ++;
			earth_N_S = *str_status;	
			str_status ++;
			str_status ++;
			str_point = strstr(str_status, ",");
			while(str_status != str_point){
				if(*str_status == '.'){
					latitude[gps_i] = '.';
					gps_i ++ ;
					str_status ++;	
				}
				latitude[gps_i] = *str_status - '0';
				gps_i ++;
				str_status ++;
			}
			gps_i = 0;
			str_point ++;
			earth_E_W = *str_point;
			str_point ++;
			str_point ++;
			right = *str_point;
			str_point ++;
			str_point ++;
			str_status = strstr(str_point, ",");
			while(str_point != str_status){
				satellite[gps_i] = *str_point - '0';
				gps_i ++;
				str_point ++;
			}
			gps_i = 0;
			str_status ++;
			str_point = strstr(str_status, ",");
			while(str_status != str_point){
				if(*str_status == '.'){
					precision[gps_i] = *str_status;
					gps_i ++;
					str_status ++;
				}
				precision[gps_i] = *str_status;
				gps_i ++;
				str_status ++;
			}
			gps_i = 0;
			str_point ++;
			str_status = strstr(str_point, ",");
			while(str_point != str_status){
				if(*str_point == '.'){
					hight[gps_i] = '.';
					gps_i ++;
					str_point ++;
				}
				hight[gps_i] = *str_point - '0';
				gps_i ++;
				str_point ++;
			}	
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
			for(gps_ii = 0; gps_ii < 9; gps_ii ++){
				printf("%d",longitude[gps_ii]);
			}
			printf("%c  ",earth_N_S);
			/////////////////////latitude//////////////
			printf(" LATITUDE:");	
			for(gps_ii = 0; gps_ii < 10; gps_ii ++){
				printf("%d",latitude[gps_ii]);
			}
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
			for(gps_ii = 0; gps_ii < 4; gps_ii ++){
				printf("%d",precision[gps_ii]);
			}
			/////////////////////hight////////////////////
			printf(" HIGHT:");
			for(gps_ii = 0; gps_ii < 7; gps_ii ++){
				printf("%d\n",hight[gps_ii]);
			}
	
		}
		else{
			printf("please waiting.......... \n");
		}	
	}
	tcsetattr(fd, TCSANOW, &old_cfg );
}

int main(){
	set_uart_conifg();

return 0;	
}

