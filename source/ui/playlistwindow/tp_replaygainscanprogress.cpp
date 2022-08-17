#include "tp_replaygainscanprogress.h"
#include "ui_tp_replaygainscanprogress.h"

TP_ReplayGainScanProgress::TP_ReplayGainScanProgress(  QWidget *parent) :
    QDialog { parent }
  , ui      { new Ui::TP_ReplayGainScanProgress }
{
    ui->setupUi( this );
}

TP_ReplayGainScanProgress::~TP_ReplayGainScanProgress()
{
    delete ui;
}
