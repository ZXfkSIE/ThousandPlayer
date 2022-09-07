#include "tp_runnable_filereader.h"

#include "tp_globalfunction.h"

TP_Runnable_FileReader::TP_Runnable_FileReader( QListWidgetItem *I_item ) :
    item { I_item }
{
}

void
TP_Runnable_FileReader::run()
{
    TP::storeInformation( item );

    emit signal_onFinish();
}
