#include "ctrlpanel.h"
#include <stdlib.h>
/**-------------------------------------------------------------------
 * <B>Function</B>:    get_ctrlpanel_status
 *
 * Get controlpanel status
 *
 * @return        0 : ControlPanel is running
 * @return        1 : ControlPanel isn't running
 *
 * @n
 *------------------------------------------------------------------*/
int get_ctrlpanel_status()
// Get controlpanel status
{
    int return_status;
    int ret;
    ret = system("ps aux | grep -E \'TzcsControlPanelMain|xfce4-display-settings\' | grep -v grep > /dev/null 2>&1");
    if(WIFEXITED(ret)){
        return_status = WEXITSTATUS(ret);
        if( return_status == 0 ) {
            return_status = 0;
        }else{
            return_status = 1;
        }
    }
    return return_status;
}
