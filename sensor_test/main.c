#include "apue.h"
#include "log.h"

#define SLEEP_TIME (200*1000) //200ms
#define CHANGE_TIMES (5*60) //SLEEP_TIME*CHANGE_TIMES=200ms*5*60 = 60s
#define REPORT_TIMES (5*10) //SLEEP_TIME*REPORT_TIMES=200ms*5*10 = 10s

enum BT_STATE {
	BT_NOT_CONNECTED,
	BT_CONNECTED,
};

enum CAT1_STATE {
	CAT1_NOT_CONNECTED,
	CAT1_CONNECTED,
};

enum REPORT_STATE {
	UNREPORTED,
	REPORTED,
};

int bt_status = BT_CONNECTED;
int cat1_status = CAT1_CONNECTED;
int report_status = UNREPORTED;

int time_count = 0;
int step_count = 0;

int active_time = 0;
int relax_time = 0;

int random_100(void)
{
	return (int)(rand()%100);
}

int changed_count = 0;
void state_changed(void)
{
	int changed = rand()%4;
	switch(changed) {
	case 0:
	default:
		bt_status = BT_CONNECTED;
		cat1_status = CAT1_CONNECTED;
		break;
	case 1:
		bt_status = BT_CONNECTED;
		cat1_status = CAT1_NOT_CONNECTED;
		break;
	case 2:
		bt_status = BT_NOT_CONNECTED;
		cat1_status = CAT1_NOT_CONNECTED;
		break;
	case 3:
		bt_status = BT_NOT_CONNECTED;
		cat1_status = CAT1_CONNECTED;
		break;
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	while(1) {
		usleep(SLEEP_TIME);
		time_count++;

		// 模拟状态切换
		changed_count++;
		if (changed_count == CHANGE_TIMES) {
			state_changed();
			changed_count = 0;
		}


		if(report_status == REPORTED || time_count >= REPORT_TIMES) {
			// 获取的模拟步数
			int step = random_100();

			step_count += step;
			active_time += step*10; //步数*每一步需要的时间
			relax_time += (time_count * SLEEP_TIME) - step*10; //一定时间 - active_time

			if (bt_status == BT_CONNECTED) {
				LOG_DBG("bt steps:%d active_time:%d relax_time:%d", step_count, active_time, relax_time);

				step_count = 0;
				active_time = 0;
				relax_time = 0;
			} else if (bt_status == BT_NOT_CONNECTED && cat1_status == CAT1_CONNECTED) {
				if (step_count >= 200) {
					LOG_DBG("cat1 steps:%d active_time:%d relax_time:%d", step_count, active_time, relax_time);
					
					step_count = 0;
					active_time = 0;
					relax_time = 0;
				}
			} else if (bt_status == BT_NOT_CONNECTED && cat1_status == CAT1_NOT_CONNECTED) {
				LOG_DBG("cat1 not connected");
			} else {
				LOG_DBG("not connected");
			}

			time_count = 0;
		}
	}
}
