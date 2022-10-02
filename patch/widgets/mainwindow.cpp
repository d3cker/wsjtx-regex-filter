//---------------------------------------------------------- MainWindow
#include "mainwindow.h"

#include <cinttypes>
#include <cstring>
#include <cmath>
#include <limits>
#include <functional>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <fftw3.h>
#include <QApplication>
#include <QStringListModel>
#include <QSettings>
#include <QKeyEvent>
#include <QProcessEnvironment>
#include <QSharedMemory>
#include <QFileDialog>
#include <QTextBlock>
#include <QProgressBar>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QRegExp>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QtConcurrent/QtConcurrentRun>
#include <QProgressDialog>
#include <QHostInfo>
#include <QVector>
#include <QCursor>
#include <QToolTip>
#include <QAction>
#include <QButtonGroup>
#include <QActionGroup>
#include <QSplashScreen>
#include <QUdpSocket>
#include <QAbstractItemView>
#include <QInputDialog>
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
#include <QRandomGenerator>
#endif

#include "helper_functions.h"
#include "revision_utils.hpp"
#include "qt_helpers.hpp"
#include "Network/NetworkAccessManager.hpp"
#include "Audio/soundout.h"
#include "Audio/soundin.h"
#include "Modulator/Modulator.hpp"
#include "Detector/Detector.hpp"
#include "plotter.h"
#include "echoplot.h"
#include "echograph.h"
#include "fastplot.h"
#include "fastgraph.h"
#include "about.h"
#include "messageaveraging.h"
#include "colorhighlighting.h"
#include "widegraph.h"
#include "sleep.h"
#include "logqso.h"
#include "Decoder/decodedtext.h"
#include "Radio.hpp"
#include "models/Bands.hpp"
#include "Transceiver/TransceiverFactory.hpp"
#include "models/StationList.hpp"
#include "validators/LiveFrequencyValidator.hpp"
#include "Network/MessageClient.hpp"
#include "Network/wsprnet.h"
#include "signalmeter.h"
#include "HelpTextWindow.hpp"
#include "SampleDownloader.hpp"
#include "Audio/BWFFile.hpp"
#include "MultiSettings.hpp"
#include "validators/MaidenheadLocatorValidator.hpp"
#include "validators/CallsignValidator.hpp"
#include "EqualizationToolsDialog.hpp"
#include "Network/LotWUsers.hpp"
#include "logbook/AD1CCty.hpp"
#include "models/FoxLog.hpp"
#include "models/CabrilloLog.hpp"
#include "FoxLogWindow.hpp"
#include "CabrilloLogWindow.hpp"
#include "ExportCabrillo.h"
#include "ui_mainwindow.h"
#include "moc_mainwindow.cpp"

extern "C" {
  //----------------------------------------------------- C and Fortran routines
  void symspec_(struct dec_data *, int* k, double* trperiod, int* nsps, int* ingain,
                bool* bLowSidelobes, int* minw, float* px, float s[], float* df3,
                int* nhsym, int* npts8, float *m_pxmax, int* npct);

  void hspec_(short int d2[], int* k, int* nutc0, int* ntrperiod, int* nrxfreq, int* ntol,
              bool* bmsk144, bool* btrain, double const pcoeffs[], int* ingain,
              char const * mycall, char const * hiscall, bool* bshmsg, bool* bswl,
              char const * ddir, float green[],
              float s[], int* jh, float *pxmax, float *rmsNoGain, char line[],
              fortran_charlen_t, fortran_charlen_t, fortran_charlen_t, fortran_charlen_t);

  void genft8_(char* msg, int* i3, int* n3, char* msgsent, char ft8msgbits[],
               int itone[], fortran_charlen_t, fortran_charlen_t);

  void genft4_(char* msg, int* ichk, char* msgsent, char ft4msgbits[], int itone[],
               fortran_charlen_t, fortran_charlen_t);

  void genfst4_(char* msg, int* ichk, char* msgsent, char fst4msgbits[],
                 int itone[], int* iwspr, fortran_charlen_t, fortran_charlen_t);

  void gen_ft8wave_(int itone[], int* nsym, int* nsps, float* bt, float* fsample, float* f0,
                    float xjunk[], float wave[], int* icmplx, int* nwave);

  void gen_ft4wave_(int itone[], int* nsym, int* nsps, float* fsample, float* f0,
                    float xjunk[], float wave[], int* icmplx, int* nwave);

  void gen_fst4wave_(int itone[], int* nsym, int* nsps, int* nwave, float* fsample,
                       int* hmod, float* f0, int* icmplx, float xjunk[], float wave[]);

  void genwave_(int itone[], int* nsym, int* nsps, int* nwave, float* fsample,
                       int* hmod, float* f0, int* icmplx, float xjunk[], float wave[]);

  void gen4_(char* msg, int* ichk, char* msgsent, int itone[],
               int* itext, fortran_charlen_t, fortran_charlen_t);

  void gen9_(char* msg, int* ichk, char* msgsent, int itone[],
               int* itext, fortran_charlen_t, fortran_charlen_t);

  void genmsk_128_90_(char* msg, int* ichk, char* msgsent, int itone[], int* itype,
                      fortran_charlen_t, fortran_charlen_t);

  void gen65_(char* msg, int* ichk, char* msgsent, int itone[],
              int* itext, fortran_charlen_t, fortran_charlen_t);

  void genq65_(char* msg, int* ichk, char* msgsent, int itone[],
              int* i3, int* n3, fortran_charlen_t, fortran_charlen_t);

  void genwspr_(char* msg, char* msgsent, int itone[], fortran_charlen_t, fortran_charlen_t);

  void azdist_(char* MyGrid, char* HisGrid, double* utch, int* nAz, int* nEl,
               int* nDmiles, int* nDkm, int* nHotAz, int* nHotABetter,
               fortran_charlen_t, fortran_charlen_t);

  void morse_(char* msg, int* icw, int* ncw, fortran_charlen_t);

  int ptt_(int nport, int ntx, int* iptt, int* nopen);

  void wspr_downsample_(short int d2[], int* k);

  int savec2_(char const * fname, int* TR_seconds, double* dial_freq, fortran_charlen_t);

  void avecho_( short id2[], int* dop, int* nfrit, int* nqual, float* f1,
                float* level, float* sigdb, float* snr, float* dfreq,
                float* width);

  void fast_decode_(short id2[], int narg[], double * trperiod,
                    char msg[], char mycall[], char hiscall[],
                    fortran_charlen_t, fortran_charlen_t, fortran_charlen_t);
  void degrade_snr_(short d2[], int* n, float* db, float* bandwidth);

  void wav12_(short d2[], short d1[], int* nbytes, short* nbitsam2);

  void refspectrum_(short int d2[], bool* bclearrefspec,
                    bool* brefspec, bool* buseref, const char* c_fname, fortran_charlen_t);

  void freqcal_(short d2[], int* k, int* nkhz,int* noffset, int* ntol,
                char line[], fortran_charlen_t);

  void fix_contest_msg_(char* MyGrid, char* msg, fortran_charlen_t, fortran_charlen_t);

  void calibrate_(char const * data_dir, int* iz, double* a, double* b, double* rms,
                  double* sigmaa, double* sigmab, int* irc, fortran_charlen_t);

  void foxgen_();

  void plotsave_(float swide[], int* m_w , int* m_h1, int* irow);

  void chkcall_(char* w, char* basc_call, bool cok, int len1, int len2);

  void get_ft4msg_(int* idecode, char* line, int len);

  void chk_samples_(int* m_ihsym,int* k, int* m_hsymStop);

  void save_dxbase_(char* dxbase, int len);
}

int volatile itone[MAX_NUM_SYMBOLS];   //Audio tones for all Tx symbols
int volatile itone0[MAX_NUM_SYMBOLS];  //Dummy array, data not actually used
int volatile icw[NUM_CW_SYMBOLS];        //Dits for CW ID
dec_data_t dec_data;                // for sharing with Fortran

int outBufSize;
int rc;
qint32  g_iptt {0};
wchar_t buffer[256];
float fast_green[703];
float fast_green2[703];
float fast_s[44992];                                    //44992=64*703
float fast_s2[44992];
int   fast_jh {0};
int   fast_jhpeak {0};
int   fast_jh2 {0};
int narg[15];
QVector<QColor> g_ColorTbl;

using SpecOp = Configuration::SpecialOperatingActivity;

namespace
{
  Radio::Frequency constexpr default_frequency {14076000};
  QRegExp message_alphabet {"[- @A-Za-z0-9+./?#<>;$]*"};
  // grid exact match excluding RR73
  QRegularExpression grid_regexp {"\\A(?![Rr]{2}73)[A-Ra-r]{2}[0-9]{2}([A-Xa-x]{2}){0,1}\\z"};
  auto quint32_max = std::numeric_limits<quint32>::max ();
  constexpr int N_WIDGETS {38};
  constexpr int default_rx_audio_buffer_frames {-1}; // lets Qt decide
  constexpr int default_tx_audio_buffer_frames {-1}; // lets Qt decide

  bool message_is_73 (int type, QStringList const& msg_parts)
  {
    return type >= 0
      && (((type < 6 || 7 == type)
           && (msg_parts.contains ("73") || msg_parts.contains ("RR73")))
          || (type == 6 && !msg_parts.filter ("73").isEmpty ()));
  }

  int ms_minute_error ()
  {
    auto const& now = QDateTime::currentDateTimeUtc ();
    auto const& time = now.time ();
    auto second = time.second ();
    return now.msecsTo (now.addSecs (second > 30 ? 60 - second : -second)) - time.msec ();
  }
}

//--------------------------------------------------- MainWindow constructor
MainWindow::MainWindow(QDir const& temp_directory, bool multiple,
                       MultiSettings * multi_settings, QSharedMemory *shdmem,
                       unsigned downSampleFactor,
                       QSplashScreen * splash, QProcessEnvironment const& env, QWidget *parent) :
  MultiGeometryWidget {parent},
  m_env {env},
  m_network_manager {this},
  m_valid {true},
  m_splash {splash},
  m_revision {revision ()},
  m_multiple {multiple},
  m_multi_settings {multi_settings},
  m_configurations_button {0},
  m_settings {multi_settings->settings ()},
  ui(new Ui::MainWindow),
  m_config {&m_network_manager, temp_directory, m_settings, &m_logBook, this},
  m_logBook {&m_config},
  m_WSPR_band_hopping {m_settings, &m_config, this},
  m_WSPR_tx_next {false},
  m_rigErrorMessageBox {MessageBox::Critical, tr ("Rig Control Error")
      , MessageBox::Cancel | MessageBox::Ok | MessageBox::Retry},
  m_wideGraph (new WideGraph(m_settings)),
  m_echoGraph (new EchoGraph(m_settings)),
  m_fastGraph (new FastGraph(m_settings)),
  // no parent so that it has a taskbar icon
  m_logDlg (new LogQSO (program_title (), m_settings, &m_config, &m_logBook, nullptr)),
  m_lastDialFreq {0},
  m_dialFreqRxWSPR {0},
  m_detector {new Detector {RX_SAMPLE_RATE, double(NTMAX), downSampleFactor}},
  m_FFTSize {6192 / 2},         // conservative value to avoid buffer overruns
  m_soundInput {new SoundInput},
  m_modulator {new Modulator {TX_SAMPLE_RATE, NTMAX}},
  m_soundOutput {new SoundOutput},
  m_rx_audio_buffer_frames {0},
  m_tx_audio_buffer_frames {0},
  m_msErase {0},
  m_secBandChanged {0},
  m_freqNominal {0},
  m_freqTxNominal {0},
  m_reverse_Doppler {"1" == env.value ("WSJT_REVERSE_DOPPLER", "0")},
  m_tRemaining {0.},
  m_TRperiod {60.0},
  m_DTtol {3.0},
  m_waterfallAvg {1},
  m_ntx {1},
  m_gen_message_is_cq {false},
  m_send_RR73 {false},
  m_XIT {0},
  m_sec0 {-1},
  m_RxLog {1},      //Write Date and Time to RxLog
  m_nutc0 {999999},
  m_ntr {0},
  m_tx {0},
  m_inGain {0},
  m_secID {0},
  m_idleMinutes {0},
  m_nSubMode {0},
  m_nclearave {1},
  m_nWSPRdecodes {0},
  m_k0 {9999999},
  m_nPick {0},
  m_frequency_list_fcal_iter {m_config.frequencies ()->begin ()},
  m_nTx73 {0},
  m_btxok {false},
  m_diskData {false},
  m_loopall {false},
  m_txFirst {false},
  m_auto {false},
  m_restart {false},
  m_startAnother {false},
  m_saveDecoded {false},
  m_saveAll {false},
  m_widebandDecode {false},
  m_dataAvailable {false},
  m_decodedText2 {false},
  m_freeText {false},
  m_sentFirst73 {false},
  m_currentMessageType {-1},
  m_lastMessageType {-1},
  m_bShMsgs {false},
  m_bSWL {false},
  m_uploading {false},
  m_grid6 {false},
  m_tuneup {false},
  m_bTxTime {false},
  m_rxDone {true},
  m_bSimplex {false},
  m_bEchoTxOK {false},
  m_bTransmittedEcho {false},
  m_bEchoTxed {false},
  m_bFastDecodeCalled {false},
  m_bDoubleClickAfterCQnnn {false},
  m_bRefSpec {false},
  m_bClearRefSpec {false},
  m_bTrain {false},
  m_bAutoReply {false},
  m_QSOProgress {CALLING},
  m_ihsym {0},
  m_nzap {0},
  m_px {0.0},
  m_iptt0 {0},
  m_btxok0 {false},
  m_nsendingsh {0},
  m_onAirFreq0 {0.0},
  m_first_error {true},
  tx_status_label {tr ("Receiving")},
  wsprNet {new WSPRNet {&m_network_manager, this}},
  m_baseCall {Radio::base_callsign (m_config.my_callsign ())},
  m_appDir {QApplication::applicationDirPath ()},
  m_cqStr {""},
  m_palette {"Linrad"},
  m_mode {"JT9"},
  m_rpt {"-15"},
  m_pfx {
    "1A", "1S",
      "3A", "3B6", "3B8", "3B9", "3C", "3C0", "3D2", "3D2C",
      "3D2R", "3DA", "3V", "3W", "3X", "3Y", "3YB", "3YP",
      "4J", "4L", "4S", "4U1I", "4U1U", "4W", "4X",
      "5A", "5B", "5H", "5N", "5R", "5T", "5U", "5V", "5W", "5X", "5Z",
      "6W", "6Y",
      "7O", "7P", "7Q", "7X",
      "8P", "8Q", "8R",
      "9A", "9G", "9H", "9J", "9K", "9L", "9M2", "9M6", "9N",
      "9Q", "9U", "9V", "9X", "9Y",
      "A2", "A3", "A4", "A5", "A6", "A7", "A9", "AP",
      "BS7", "BV", "BV9", "BY",
      "C2", "C3", "C5", "C6", "C9", "CE", "CE0X", "CE0Y",
      "CE0Z", "CE9", "CM", "CN", "CP", "CT", "CT3", "CU",
      "CX", "CY0", "CY9",
      "D2", "D4", "D6", "DL", "DU",
      "E3", "E4", "E5", "EA", "EA6", "EA8", "EA9", "EI", "EK",
      "EL", "EP", "ER", "ES", "ET", "EU", "EX", "EY", "EZ",
      "F", "FG", "FH", "FJ", "FK", "FKC", "FM", "FO", "FOA",
      "FOC", "FOM", "FP", "FR", "FRG", "FRJ", "FRT", "FT5W",
      "FT5X", "FT5Z", "FW", "FY",
      "M", "MD", "MI", "MJ", "MM", "MU", "MW",
      "H4", "H40", "HA", "HB", "HB0", "HC", "HC8", "HH",
      "HI", "HK", "HK0", "HK0M", "HL", "HM", "HP", "HR",
      "HS", "HV", "HZ",
      "I", "IS", "IS0",
      "J2", "J3", "J5", "J6", "J7", "J8", "JA", "JDM",
      "JDO", "JT", "JW", "JX", "JY",
      "K", "KC4", "KG4", "KH0", "KH1", "KH2", "KH3", "KH4", "KH5",
      "KH5K", "KH6", "KH7", "KH8", "KH9", "KL", "KP1", "KP2",
      "KP4", "KP5",
      "LA", "LU", "LX", "LY", "LZ",
      "OA", "OD", "OE", "OH", "OH0", "OJ0", "OK", "OM", "ON",
      "OX", "OY", "OZ",
      "P2", "P4", "PA", "PJ2", "PJ7", "PY", "PY0F", "PT0S", "PY0T", "PZ",
      "R1F", "R1M",
      "S0", "S2", "S5", "S7", "S9", "SM", "SP", "ST", "SU",
      "SV", "SVA", "SV5", "SV9",
      "T2", "T30", "T31", "T32", "T33", "T5", "T7", "T8", "T9", "TA",
      "TF", "TG", "TI", "TI9", "TJ", "TK", "TL", "TN", "TR", "TT",
      "TU", "TY", "TZ",
      "UA", "UA2", "UA9", "UK", "UN", "UR",
      "V2", "V3", "V4", "V5", "V6", "V7", "V8", "VE", "VK", "VK0H",
      "VK0M", "VK9C", "VK9L", "VK9M", "VK9N", "VK9W", "VK9X", "VP2E",
      "VP2M", "VP2V", "VP5", "VP6", "VP6D", "VP8", "VP8G", "VP8H",
      "VP8O", "VP8S", "VP9", "VQ9", "VR", "VU", "VU4", "VU7",
      "XE", "XF4", "XT", "XU", "XW", "XX9", "XZ",
      "YA", "YB", "YI", "YJ", "YK", "YL", "YN", "YO", "YS", "YU", "YV", "YV0",
      "Z2", "Z3", "ZA", "ZB", "ZC4", "ZD7", "ZD8", "ZD9", "ZF", "ZK1N",
      "ZK1S", "ZK2", "ZK3", "ZL", "ZL7", "ZL8", "ZL9", "ZP", "ZS", "ZS8"
      },
  m_sfx {"P",  "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "A"},
  mem_jt9 {shdmem},
  m_downSampleFactor (downSampleFactor),
  m_audioThreadPriority (QThread::HighPriority),
  m_bandEdited {false},
  m_splitMode {false},
  m_monitoring {false},
  m_tx_when_ready {false},
  m_transmitting {false},
  m_tune {false},
  m_tx_watchdog {false},
  m_block_pwr_tooltip {false},
  m_PwrBandSetOK {true},
  m_lastMonitoredFrequency {default_frequency},
  m_toneSpacing {0.},
  m_firstDecode {0},
  m_optimizingProgress {"Optimizing decoder FFTs for your CPU.\n"
      "Please be patient,\n"
      "this may take a few minutes", QString {}, 0, 1, this},
  m_messageClient {new MessageClient {QApplication::applicationName (),
        version (), revision (),
        m_config.udp_server_name (), m_config.udp_server_port (),
        m_config.udp_interface_names (), m_config.udp_TTL (),
        this}},
  m_psk_Reporter {&m_config, QString {"WSJT-X v" + version () + " " + m_revision}.simplified ()},
  m_manual {&m_network_manager},
  m_block_udp_status_updates {false}
{
  ui->setupUi(this);
  setUnifiedTitleAndToolBarOnMac (true);
  createStatusBar();
  add_child_to_event_filter (this);
  ui->dxGridEntry->setValidator (new MaidenheadLocatorValidator {this});
  ui->dxCallEntry->setValidator (new CallsignValidator {this});
  ui->sbTR->values ({5, 10, 15, 30, 60, 120, 300, 900, 1800});
  ui->sbTR_FST4W->values ({120, 300, 900, 1800});
  ui->decodedTextBrowser->set_configuration (&m_config, true);
  ui->decodedTextBrowser2->set_configuration (&m_config);

  m_optimizingProgress.setWindowModality (Qt::WindowModal);
  m_optimizingProgress.setAutoReset (false);
  m_optimizingProgress.setMinimumDuration (15000); // only show after 15s delay

  // Closedown.
  connect (ui->actionExit, &QAction::triggered, this, &QMainWindow::close);

  // parts of the rig error message box that are fixed
  m_rigErrorMessageBox.setInformativeText (tr ("Do you want to reconfigure the radio interface?"));
  m_rigErrorMessageBox.setDefaultButton (MessageBox::Ok);

  // start audio thread and hook up slots & signals for shutdown management
  // these objects need to be in the audio thread so that invoking
  // their slots is done in a thread safe way
  m_soundOutput->moveToThread (&m_audioThread);
  m_modulator->moveToThread (&m_audioThread);
  m_soundInput->moveToThread (&m_audioThread);
  m_detector->moveToThread (&m_audioThread);
  bool ok;
  auto buffer_size = env.value ("WSJT_RX_AUDIO_BUFFER_FRAMES", "0").toInt (&ok);
  m_rx_audio_buffer_frames = ok && buffer_size ? buffer_size : default_rx_audio_buffer_frames;
  buffer_size = env.value ("WSJT_TX_AUDIO_BUFFER_FRAMES", "0").toInt (&ok);
  m_tx_audio_buffer_frames = ok && buffer_size ? buffer_size : default_tx_audio_buffer_frames;

  // hook up sound output stream slots & signals and disposal
  connect (this, &MainWindow::initializeAudioOutputStream, m_soundOutput, &SoundOutput::setFormat);
  connect (m_soundOutput, &SoundOutput::error, this, &MainWindow::showSoundOutError);
  connect (m_soundOutput, &SoundOutput::error, &m_config, &Configuration::invalidate_audio_output_device);
  // connect (m_soundOutput, &SoundOutput::status, this, &MainWindow::showStatusMessage);
  connect (this, &MainWindow::outAttenuationChanged, m_soundOutput, &SoundOutput::setAttenuation);
  connect (&m_audioThread, &QThread::finished, m_soundOutput, &QObject::deleteLater);

  // hook up Modulator slots and disposal
  connect (this, &MainWindow::transmitFrequency, m_modulator, &Modulator::setFrequency);
  connect (this, &MainWindow::endTransmitMessage, m_modulator, &Modulator::stop);
  connect (this, &MainWindow::tune, m_modulator, &Modulator::tune);
  connect (this, &MainWindow::sendMessage, m_modulator, &Modulator::start);
  connect (&m_audioThread, &QThread::finished, m_modulator, &QObject::deleteLater);

  // hook up the audio input stream signals, slots and disposal
  connect (this, &MainWindow::startAudioInputStream, m_soundInput, &SoundInput::start);
  connect (this, &MainWindow::suspendAudioInputStream, m_soundInput, &SoundInput::suspend);
  connect (this, &MainWindow::resumeAudioInputStream, m_soundInput, &SoundInput::resume);
  connect (this, &MainWindow::reset_audio_input_stream, m_soundInput, &SoundInput::reset);
  connect (this, &MainWindow::finished, m_soundInput, &SoundInput::stop);
  connect(m_soundInput, &SoundInput::error, this, &MainWindow::showSoundInError);
  connect(m_soundInput, &SoundInput::error, &m_config, &Configuration::invalidate_audio_input_device);
  // connect(m_soundInput, &SoundInput::status, this, &MainWindow::showStatusMessage);
  connect (&m_audioThread, &QThread::finished, m_soundInput, &QObject::deleteLater);

  connect (this, &MainWindow::finished, this, &MainWindow::close);

  // hook up the detector signals, slots and disposal
  connect (this, &MainWindow::FFTSize, m_detector, &Detector::setBlockSize);
  connect(m_detector, &Detector::framesWritten, this, &MainWindow::dataSink);
  connect (&m_audioThread, &QThread::finished, m_detector, &QObject::deleteLater);

  // setup the waterfall
  connect(m_wideGraph.data (), SIGNAL(freezeDecode2(int)),this,SLOT(freezeDecode(int)));
  connect(m_wideGraph.data (), SIGNAL(f11f12(int)),this,SLOT(bumpFqso(int)));
  connect(m_wideGraph.data (), SIGNAL(setXIT2(int)),this,SLOT(setXIT(int)));

  connect (m_fastGraph.data (), &FastGraph::fastPick, this, &MainWindow::fastPick);

  connect (this, &MainWindow::finished, m_wideGraph.data (), &WideGraph::close);
  connect (this, &MainWindow::finished, m_echoGraph.data (), &EchoGraph::close);
  connect (this, &MainWindow::finished, m_fastGraph.data (), &FastGraph::close);

  // setup the log QSO dialog
  connect (m_logDlg.data (), &LogQSO::acceptQSO, this, &MainWindow::acceptQSO);
  connect (this, &MainWindow::finished, m_logDlg.data (), &LogQSO::close);

  // hook up the log book
  connect (&m_logBook, &LogBook::finished_loading, [this] (int record_count, QString const& error) {
      if (error.size ())
        {
          MessageBox::warning_message (this, tr ("Error Scanning ADIF Log"), error);
        }
      else
        {
          showStatusMessage (tr ("Scanned ADIF log, %1 worked before records created").arg (record_count));
        }
    });

  // Network message handlers
  m_messageClient->enable (m_config.accept_udp_requests ());
  connect (m_messageClient, &MessageClient::clear_decodes, [this] (quint8 window) {
      ++window;
      if (window & 1)
        {
          ui->decodedTextBrowser->erase ();
        }
      if (window & 2)
        {
          ui->decodedTextBrowser2->erase ();
        }
    });
  connect (m_messageClient, &MessageClient::reply, this, &MainWindow::replyToCQ);
  connect (m_messageClient, &MessageClient::close, this, &MainWindow::close);
  connect (m_messageClient, &MessageClient::replay, this, &MainWindow::replayDecodes);
  connect (m_messageClient, &MessageClient::location, this, &MainWindow::locationChange);
  connect (m_messageClient, &MessageClient::halt_tx, [this] (bool auto_only) {
      if (auto_only) {
        if (ui->autoButton->isChecked ()) {
          ui->autoButton->click();
        }
      } else {
        ui->stopTxButton->click();
      }
    });
  connect (m_messageClient, &MessageClient::error, this, &MainWindow::networkError);
  connect (m_messageClient, &MessageClient::free_text, [this] (QString const& text, bool send) {
      tx_watchdog (false);
      // send + non-empty text means set and send the free text
      // message, !send + non-empty text means set the current free
      // text message, send + empty text means send the current free
      // text message without change, !send + empty text means clear
      // the current free text message
      if (0 == ui->tabWidget->currentIndex ()) {
        if (!text.isEmpty ()) {
          ui->tx5->setCurrentText (text);
        }
        if (send) {
          ui->txb5->click ();
        } else if (text.isEmpty ()) {
          ui->tx5->setCurrentText (text);
        }
      }
      QApplication::alert (this);
    });

  connect (m_messageClient, &MessageClient::highlight_callsign, ui->decodedTextBrowser, &DisplayText::highlight_callsign);
  connect (m_messageClient, &MessageClient::switch_configuration, m_multi_settings, &MultiSettings::select_configuration);
  connect (m_messageClient, &MessageClient::configure, this, &MainWindow::remote_configure);

  // Hook up WSPR band hopping
  connect (ui->band_hopping_schedule_push_button, &QPushButton::clicked
           , &m_WSPR_band_hopping, &WSPRBandHopping::show_dialog);
  connect (ui->sbTxPercent, static_cast<void (QSpinBox::*) (int)> (&QSpinBox::valueChanged)
           , &m_WSPR_band_hopping, &WSPRBandHopping::set_tx_percent);

  on_EraseButton_clicked ();

  QActionGroup* modeGroup = new QActionGroup(this);
  ui->actionFST4->setActionGroup(modeGroup);
  ui->actionFST4W->setActionGroup(modeGroup);
  ui->actionFT4->setActionGroup(modeGroup);
  ui->actionFT8->setActionGroup(modeGroup);
  ui->actionJT9->setActionGroup(modeGroup);
  ui->actionJT65->setActionGroup(modeGroup);
  ui->actionJT4->setActionGroup(modeGroup);
  ui->actionWSPR->setActionGroup(modeGroup);
  ui->actionEcho->setActionGroup(modeGroup);
  ui->actionMSK144->setActionGroup(modeGroup);
  ui->actionQ65->setActionGroup(modeGroup);
  ui->actionFreqCal->setActionGroup(modeGroup);

  QActionGroup* saveGroup = new QActionGroup(this);
  ui->actionNone->setActionGroup(saveGroup);
  ui->actionSave_decoded->setActionGroup(saveGroup);
  ui->actionSave_all->setActionGroup(saveGroup);

  QActionGroup* DepthGroup = new QActionGroup(this);
  ui->actionQuickDecode->setActionGroup(DepthGroup);
  ui->actionMediumDecode->setActionGroup(DepthGroup);
  ui->actionDeepestDecode->setActionGroup(DepthGroup);

  connect (ui->download_samples_action, &QAction::triggered, [this] () {
      if (!m_sampleDownloader)
        {
          m_sampleDownloader.reset (new SampleDownloader {m_settings, &m_config, &m_network_manager, this});
        }
      m_sampleDownloader->show ();
    });

  connect (ui->view_phase_response_action, &QAction::triggered, [this] () {
      if (!m_equalizationToolsDialog)
        {
          m_equalizationToolsDialog.reset (new EqualizationToolsDialog {m_settings, m_config.writeable_data_dir (), m_phaseEqCoefficients, this});
          connect (m_equalizationToolsDialog.data (), &EqualizationToolsDialog::phase_equalization_changed,
                   [this] (QVector<double> const& coeffs) {
                     m_phaseEqCoefficients = coeffs;
                   });
        }
      m_equalizationToolsDialog->show ();
    });

  connect (&m_config.lotw_users (), &LotWUsers::LotW_users_error, this, [this] (QString const& reason) {
      MessageBox::warning_message (this, tr ("Error Loading LotW Users Data"), reason);
    }, Qt::QueuedConnection);

  QButtonGroup* txMsgButtonGroup = new QButtonGroup {this};
  txMsgButtonGroup->addButton(ui->txrb1,1);
  txMsgButtonGroup->addButton(ui->txrb2,2);
  txMsgButtonGroup->addButton(ui->txrb3,3);
  txMsgButtonGroup->addButton(ui->txrb4,4);
  txMsgButtonGroup->addButton(ui->txrb5,5);
  txMsgButtonGroup->addButton(ui->txrb6,6);
  set_dateTimeQSO(-1);
  connect(txMsgButtonGroup,SIGNAL(buttonClicked(int)),SLOT(set_ntx(int)));
  connect (ui->decodedTextBrowser, &DisplayText::selectCallsign, this, &MainWindow::doubleClickOnCall2);
  connect (ui->decodedTextBrowser2, &DisplayText::selectCallsign, this, &MainWindow::doubleClickOnCall);
  connect (ui->textBrowser4, &DisplayText::selectCallsign, this, &MainWindow::doubleClickOnFoxQueue);
  connect (ui->decodedTextBrowser, &DisplayText::erased, this, &MainWindow::band_activity_cleared);
  connect (ui->decodedTextBrowser2, &DisplayText::erased, this, &MainWindow::rx_frequency_activity_cleared);

  // initialize decoded text font and hook up font change signals
  // defer initialization until after construction otherwise menu
  // fonts do not get set
  QTimer::singleShot (0, this, SLOT (initialize_fonts ()));
  connect (&m_config, &Configuration::text_font_changed, [this] (QFont const& font) {
      set_application_font (font);
    });
  connect (&m_config, &Configuration::decoded_text_font_changed, [this] (QFont const& font) {
      setDecodedTextFont (font);
    });

  setWindowTitle (program_title ());

  connect(&proc_jt9, &QProcess::readyReadStandardOutput, this, &MainWindow::readFromStdout);
#if QT_VERSION < QT_VERSION_CHECK (5, 6, 0)
  connect(&proc_jt9, static_cast<void (QProcess::*) (QProcess::ProcessError)> (&QProcess::error),
          [this] (QProcess::ProcessError error) {
            subProcessError (&proc_jt9, error);
          });
#else
  connect(&proc_jt9, &QProcess::errorOccurred, [this] (QProcess::ProcessError error) {
                                                 subProcessError (&proc_jt9, error);
                                               });
#endif
  connect(&proc_jt9, static_cast<void (QProcess::*) (int, QProcess::ExitStatus)> (&QProcess::finished),
          [this] (int exitCode, QProcess::ExitStatus status) {
            if (subProcessFailed (&proc_jt9, exitCode, status))
              {
                m_valid = false;          // ensures exit if still
                                          // constructing
                QTimer::singleShot (0, this, SLOT (close ()));
              }
          });
  connect(&p1, &QProcess::started, [this] () {
                                     showStatusMessage (QString {"Started: %1 \"%2\""}.arg (p1.program ()).arg (p1.arguments ().join ("\" \"")));
                                   });
  connect(&p1, &QProcess::readyReadStandardOutput, this, &MainWindow::p1ReadFromStdout);
#if QT_VERSION < QT_VERSION_CHECK (5, 6, 0)
  connect(&p1, static_cast<void (QProcess::*) (QProcess::ProcessError)> (&QProcess::error),
          [this] (QProcess::ProcessError error) {
            subProcessError (&p1, error);
          });
#else
  connect(&p1, &QProcess::errorOccurred, [this] (QProcess::ProcessError error) {
                                           subProcessError (&p1, error);
                                         });
#endif
  connect(&p1, static_cast<void (QProcess::*) (int, QProcess::ExitStatus)> (&QProcess::finished),
          [this] (int exitCode, QProcess::ExitStatus status) {
            if (subProcessFailed (&p1, exitCode, status))
              {
                m_valid = false;          // ensures exit if still
                                          // constructing
                QTimer::singleShot (0, this, SLOT (close ()));
              }
          });

#if QT_VERSION < QT_VERSION_CHECK (5, 6, 0)
  connect(&p3, static_cast<void (QProcess::*) (QProcess::ProcessError)> (&QProcess::error),
          [this] (QProcess::ProcessError error) {
#else
  connect(&p3, &QProcess::errorOccurred, [this] (QProcess::ProcessError error) {
#endif
#if !defined(Q_OS_WIN)
                                           if (QProcess::FailedToStart != error)
#else
                                           if (QProcess::Crashed != error)
#endif
                                             {
                                               subProcessError (&p3, error);
                                             }
                                         });
  connect(&p3, &QProcess::started, [this] () {
                                     showStatusMessage (QString {"Started: %1 \"%2\""}.arg (p3.program ()).arg (p3.arguments ().join ("\" \"")));
                                   });
  connect(&p3, static_cast<void (QProcess::*) (int, QProcess::ExitStatus)> (&QProcess::finished),
          [this] (int exitCode, QProcess::ExitStatus status) {
#if defined(Q_OS_WIN)
            // We forgo detecting user_hardware failures with exit
            // code 1 on Windows. This is because we use CMD.EXE to
            // run the executable. CMD.EXE returns exit code 1 when it
            // can't find the target executable.
            if (exitCode != 1)  // CMD.EXE couldn't find file to execute
#else
            // We forgo detecting user_hardware failures with exit
            // code 127 non-Windows. This is because we use /bin/sh to
            // run the executable. /bin/sh returns exit code 127 when it
            // can't find the target executable.
            if (exitCode != 127)  // /bin/sh couldn't find file to execute
#endif
              {
                subProcessFailed (&p3, exitCode, status);
              }
          });

  // hook up save WAV file exit handling
  connect (&m_saveWAVWatcher, &QFutureWatcher<QString>::finished, [this] {
      // extract the promise from the future
      auto const& result = m_saveWAVWatcher.future ().result ();
      if (!result.isEmpty ())   // error
        {
          MessageBox::critical_message (this, tr("Error Writing WAV File"), result);
        }
    });

  // Hook up working frequencies.
  ui->bandComboBox->setModel (m_config.frequencies ());
  ui->bandComboBox->setModelColumn (FrequencyList_v2::frequency_mhz_column);

  // Enable live band combo box entry validation and action.
  auto band_validator = new LiveFrequencyValidator {ui->bandComboBox
                                                    , m_config.bands ()
                                                    , m_config.frequencies ()
                                                    , &m_freqNominal
                                                    , this};
  ui->bandComboBox->setValidator (band_validator);

  // Hook up signals.
  connect (band_validator, &LiveFrequencyValidator::valid, this, &MainWindow::band_changed);
  connect (ui->bandComboBox->lineEdit (), &QLineEdit::textEdited, [this] (QString const&) {m_bandEdited = true;});

  // hook up configuration signals
  connect (&m_config, &Configuration::transceiver_update, this, &MainWindow::handle_transceiver_update);
  connect (&m_config, &Configuration::transceiver_failure, this, &MainWindow::handle_transceiver_failure);
  connect (&m_config, &Configuration::udp_server_changed, m_messageClient, &MessageClient::set_server);
  connect (&m_config, &Configuration::udp_server_port_changed, m_messageClient, &MessageClient::set_server_port);
  connect (&m_config, &Configuration::udp_TTL_changed, m_messageClient, &MessageClient::set_TTL);
  connect (&m_config, &Configuration::accept_udp_requests_changed, m_messageClient, &MessageClient::enable);
  connect (&m_config, &Configuration::enumerating_audio_devices, [this] () {
                                                                   showStatusMessage (tr ("Enumerating audio devices"));
                                                                 });

  // set up configurations menu
  connect (m_multi_settings, &MultiSettings::configurationNameChanged, [this] (QString const& name) {
      if ("Default" != name) {
        config_label.setText (name);
        config_label.show ();
      }
      else {
        config_label.hide ();
      }
      statusUpdate ();
    });
  m_multi_settings->create_menu_actions (this, ui->menuConfig);
  m_configurations_button = m_rigErrorMessageBox.addButton (tr ("Configurations...")
                                                            , QMessageBox::ActionRole);

  // set up message text validators
  ui->tx1->setValidator (new QRegExpValidator {message_alphabet, this});
  ui->tx2->setValidator (new QRegExpValidator {message_alphabet, this});
  ui->tx3->setValidator (new QRegExpValidator {message_alphabet, this});
  ui->tx4->setValidator (new QRegExpValidator {message_alphabet, this});
  ui->tx5->setValidator (new QRegExpValidator {message_alphabet, this});
  ui->tx6->setValidator (new QRegExpValidator {message_alphabet, this});

  // Free text macros model to widget hook up.
  ui->tx5->setModel (m_config.macros ());
  connect (ui->tx5->lineEdit(), &QLineEdit::editingFinished,
           [this] () {on_tx5_currentTextChanged (ui->tx5->lineEdit()->text());});
  connect(&m_guiTimer, &QTimer::timeout, this, &MainWindow::guiUpdate);
  m_guiTimer.start(100);   //### Don't change the 100 ms! ###

  ptt0Timer.setSingleShot(true);
  connect(&ptt0Timer, &QTimer::timeout, this, &MainWindow::stopTx2);

  ptt1Timer.setSingleShot(true);
  connect(&ptt1Timer, &QTimer::timeout, this, &MainWindow::startTx2);

  p1Timer.setSingleShot(true);
  connect(&p1Timer, &QTimer::timeout, this, &MainWindow::startP1);

  logQSOTimer.setSingleShot(true);
  connect(&logQSOTimer, &QTimer::timeout, this, &MainWindow::on_logQSOButton_clicked);

  tuneButtonTimer.setSingleShot(true);
  connect(&tuneButtonTimer, &QTimer::timeout, this, &MainWindow::end_tuning);

  tuneATU_Timer.setSingleShot(true);
  connect(&tuneATU_Timer, &QTimer::timeout, this, &MainWindow::stopTuneATU);

  killFileTimer.setSingleShot(true);
  connect(&killFileTimer, &QTimer::timeout, this, &MainWindow::killFile);

  uploadTimer.setSingleShot(true);
  connect(&uploadTimer, &QTimer::timeout, [this] () {uploadWSPRSpots ();});

  TxAgainTimer.setSingleShot(true);
  connect(&TxAgainTimer, SIGNAL(timeout()), this, SLOT(TxAgain()));

  connect(m_wideGraph.data (), SIGNAL(setFreq3(int,int)),this,
          SLOT(setFreq4(int,int)));

  decodeBusy(false);

  m_msg[0][0]=0;
  ui->labDXped->setVisible(false);
  ui->labDXped->setStyleSheet("QLabel {background-color: red; color: white;}");

  char const * const power[] = {"1 mW","2 mW","5 mW","10 mW","20 mW","50 mW","100 mW","200 mW","500 mW",
                  "1 W","2 W","5 W","10 W","20 W","50 W","100 W","200 W","500 W","1 kW"};
  for(auto i = 0u; i < sizeof power / sizeof power[0]; ++i)  { //Initialize dBm values
    auto dBm = int ((10. * i / 3.) + .5);
    ui->TxPowerComboBox->addItem (QString {"%1 dBm  %2"}.arg (dBm).arg (power[i]), dBm);
  }

  m_dateTimeRcvdRR73=QDateTime::currentDateTimeUtc();
  m_dateTimeSentTx3=QDateTime::currentDateTimeUtc();

  ui->labAz->setStyleSheet("border: 0px;");
  ui->labAz->setText("");
  auto t = "UTC   dB   DT Freq    " + tr ("Message");
  ui->lh_decodes_headings_label->setText(t);
  ui->rh_decodes_headings_label->setText(t);
  readSettings();            //Restore user's setup parameters
  m_audioThread.start (m_audioThreadPriority);

#ifdef WIN32
  if (!m_multiple)
    {
      while(true)
        {
          int iret=killbyname("jt9.exe");
          if(iret == 603) break;
          if(iret != 0)
            MessageBox::warning_message (this, tr ("Error Killing jt9.exe Process")
                                         , tr ("KillByName return code: %1")
                                         .arg (iret));
        }
    }
#endif

  {
    //delete any .quit file that might have been left lying around
    //since its presence will cause jt9 to exit a soon as we start it
    //and decodes will hang
    QFile quitFile {m_config.temp_dir ().absoluteFilePath (".quit")};
    while (quitFile.exists ())
      {
        if (!quitFile.remove ())
          {
            MessageBox::query_message (this, tr ("Error removing \"%1\"").arg (quitFile.fileName ())
                                       , tr ("Click OK to retry"));
          }
      }
  }

  to_jt9(0,0,0);     //initialize IPC variables

  QStringList jt9_args {
    "-s", QApplication::applicationName () // shared memory key,
                                           // includes rig
#ifdef NDEBUG
      , "-w", "1"               //FFTW patience - release
#else
      , "-w", "1"               //FFTW patience - debug builds for speed
#endif
      // The number  of threads for  FFTW specified here is  chosen as
      // three because  that gives  the best  throughput of  the large
      // FFTs used  in jt9.  The count  is the minimum of  (the number
      // available CPU threads less one) and three.  This ensures that
      // there is always at least one free CPU thread to run the other
      // mode decoder in parallel.
      , "-m", QString::number (qMin (qMax (QThread::idealThreadCount () - 1, 1), 3)) //FFTW threads

      , "-e", QDir::toNativeSeparators (m_appDir)
      , "-a", QDir::toNativeSeparators (m_config.writeable_data_dir ().absolutePath ())
      , "-t", QDir::toNativeSeparators (m_config.temp_dir ().absolutePath ())
      };
  QProcessEnvironment new_env {m_env};
  new_env.insert ("OMP_STACKSIZE", "4M");
  proc_jt9.setProcessEnvironment (new_env);
  proc_jt9.start(QDir::toNativeSeparators (m_appDir) + QDir::separator () +
          "jt9", jt9_args, QIODevice::ReadWrite | QIODevice::Unbuffered);

  auto fname {QDir::toNativeSeparators(m_config.writeable_data_dir ().absoluteFilePath ("wsjtx_wisdom.dat"))};
  fftwf_import_wisdom_from_filename (fname.toLocal8Bit ());

  m_ntx = 6;
  ui->txrb6->setChecked(true);

  connect (&m_wav_future_watcher, &QFutureWatcher<void>::finished, this, &MainWindow::diskDat);

  connect(&watcher3, SIGNAL(finished()),this,SLOT(fast_decode_done()));
  if (!m_config.audio_input_device ().isNull ())
    {
      Q_EMIT startAudioInputStream (m_config.audio_input_device ()
                                    , m_rx_audio_buffer_frames
                                    , m_detector, m_downSampleFactor, m_config.audio_input_channel ());
    }
  if (!m_config.audio_output_device ().isNull ())
    {
      Q_EMIT initializeAudioOutputStream (m_config.audio_output_device ()
                                          , AudioDevice::Mono == m_config.audio_output_channel () ? 1 : 2
                                          , m_tx_audio_buffer_frames);
    }
  Q_EMIT transmitFrequency (ui->TxFreqSpinBox->value () - m_XIT);

  enable_DXCC_entity (m_config.DXCC ());  // sets text window proportions and (re)inits the logbook

  // this must be done before initializing the mode as some modes need
  // to turn off split on the rig e.g. WSPR
  m_config.transceiver_online ();
  bool vhf {m_config.enable_VHF_features ()};

  ui->txFirstCheckBox->setChecked(m_txFirst);
  morse_(const_cast<char *> (m_config.my_callsign ().toLatin1().constData()),
         const_cast<int *> (icw), &m_ncw, m_config.my_callsign ().length());
  on_actionWide_Waterfall_triggered();
  ui->cbShMsgs->setChecked(m_bShMsgs);
  ui->cbSWL->setChecked(m_bSWL);
  if(m_bFast9) m_bFastMode=true;
  ui->cbFast9->setChecked(m_bFast9 or m_bFastMode);

  set_mode (m_mode);
  if(m_mode=="Echo") monitor(false);   //Don't auto-start Monitor in Echo mode.
  ui->sbSubmode->setValue (vhf ? m_nSubMode : 0);  //Submodes require VHF features
  if(m_mode=="MSK144") {
    Q_EMIT transmitFrequency (1000.0);
  } else {
    Q_EMIT transmitFrequency (ui->TxFreqSpinBox->value() - m_XIT);
  }
  m_saveDecoded=ui->actionSave_decoded->isChecked();
  m_saveAll=ui->actionSave_all->isChecked();
  ui->TxPowerComboBox->setCurrentIndex(int(.3 * m_dBm + .2));
  ui->cbUploadWSPR_Spots->setChecked(m_uploadWSPRSpots);
  if((m_ndepth&7)==1) ui->actionQuickDecode->setChecked(true);
  if((m_ndepth&7)==2) ui->actionMediumDecode->setChecked(true);
  if((m_ndepth&7)==3) ui->actionDeepestDecode->setChecked(true);
  ui->actionInclude_averaging->setChecked(m_ndepth&16);
  ui->actionInclude_correlation->setChecked(m_ndepth&32);
  ui->actionEnable_AP_DXcall->setChecked(m_ndepth&64);
  ui->actionAuto_Clear_Avg->setChecked(m_ndepth&128);

  m_UTCdisk=-1;
  m_fCPUmskrtd=0.0;
  m_bFastDone=false;
  m_bAltV=false;
  m_bNoMoreFiles=false;
  m_bDoubleClicked=false;
  m_bCallingCQ=false;
  m_bCheckedContest=false;
  m_bDisplayedOnce=false;
  m_wait=0;
  m_isort=-3;
  m_max_dB=70;
  m_CQtype="CQ";
  fixStop();
  VHF_features_enabled(m_config.enable_VHF_features());
  m_wideGraph->setVHF(m_config.enable_VHF_features());

  connect( wsprNet, SIGNAL(uploadStatus(QString)), this, SLOT(uploadResponse(QString)));

  statusChanged();

  m_fastGraph->setMode(m_mode);
  m_wideGraph->setMode(m_mode);

  connect (&minuteTimer, &QTimer::timeout, this, &MainWindow::on_the_minute);
  minuteTimer.setSingleShot (true);
  minuteTimer.start (ms_minute_error () + 60 * 1000);

  connect (&splashTimer, &QTimer::timeout, this, &MainWindow::splash_done);
  splashTimer.setSingleShot (true);
  splashTimer.start (20 * 1000);

  if(QCoreApplication::applicationVersion().contains("-devel") or
     QCoreApplication::applicationVersion().contains("-rc")) {
    QTimer::singleShot (0, this, SLOT (not_GA_warning_message ()));
  }

  ui->pbBestSP->setVisible(m_mode=="FT4");

// this must be the last statement of constructor
  if (!m_valid) throw std::runtime_error {"Fatal initialization exception"};
}

void MainWindow::not_GA_warning_message ()
{
  // MessageBox::critical_message (this,
  //                               "This is a pre-release version of WSJT-X " + version (false) + " made\n"
  //                               "available for testing purposes.  By design it will\n"
  //                               "be nonfunctional after Nov 30, 2021.");
  // auto now = QDateTime::currentDateTimeUtc ();
  // if (now >= QDateTime {{2021, 11, 30}, {23, 59, 59, 999}, Qt::UTC}) {
  //   Q_EMIT finished ();
  // }
}

void MainWindow::initialize_fonts ()
{
  set_application_font (m_config.text_font ());
  setDecodedTextFont (m_config.decoded_text_font ());
}

void MainWindow::splash_done ()
{
  m_splash && m_splash->close ();
}

void MainWindow::on_the_minute ()
{
  if (minuteTimer.isSingleShot ())
    {
      minuteTimer.setSingleShot (false);
      minuteTimer.start (60 * 1000); // run free
    }
  else
    {
        auto const& ms_error = ms_minute_error ();
        if (qAbs (ms_error) > 1000) // keep drift within +-1s
        {
          minuteTimer.setSingleShot (true);
          minuteTimer.start (ms_error + 60 * 1000);
        }
    }

  if (m_config.watchdog () && m_mode!="WSPR" && m_mode!="FST4W") {
    if (m_idleMinutes < m_config.watchdog ()) ++m_idleMinutes;
    update_watchdog_label ();
  } else {
    tx_watchdog (false);
  }
}

//--------------------------------------------------- MainWindow destructor
MainWindow::~MainWindow()
{
  m_astroWidget.reset ();
  auto fname {QDir::toNativeSeparators(m_config.writeable_data_dir ().absoluteFilePath ("wsjtx_wisdom.dat"))};
  fftwf_export_wisdom_to_filename (fname.toLocal8Bit ());
  m_audioThread.quit ();
  m_audioThread.wait ();
  remove_child_from_event_filter (this);
}

//-------------------------------------------------------- writeSettings()
void MainWindow::writeSettings()
{
  m_settings->beginGroup("MainWindow");
  if (ui->actionSWL_Mode->isChecked ())
    {
      m_settings->setValue ("SWLView", true);
      m_settings->setValue ("ShowMenus", ui->cbMenus->isChecked ());
      m_settings->setValue ("geometry", geometries ()[0]);
      m_settings->setValue ("SWLModeGeometry", saveGeometry ());
      m_settings->setValue ("geometryNoControls", geometries ()[2]);
    }
  else
    {
      if (ui->cbMenus->isChecked())
        {
          m_settings->setValue ("SWLView", ui->actionSWL_Mode->isChecked ());
          m_settings->setValue ("ShowMenus", true);
          m_settings->setValue ("geometry", saveGeometry ());
          m_settings->setValue ("SWLModeGeometry", geometries ()[1]);
          m_settings->setValue ("geometryNoControls", geometries ()[2]);
        }
      else
        {
          m_settings->setValue ("SWLView", ui->actionSWL_Mode->isChecked ());
          m_settings->setValue ("ShowMenus", false);
          m_settings->setValue ("geometry", geometries ()[0]);
          m_settings->setValue ("SWLModeGeometry", geometries ()[1]);
          m_settings->setValue ("geometryNoControls", saveGeometry ());
        }
    }
  m_settings->setValue ("state", saveState ());
  m_settings->setValue("MRUdir", m_path);
  m_settings->setValue("TxFirst",m_txFirst);
  m_settings->setValue("DXcall",ui->dxCallEntry->text());
  m_settings->setValue("DXgrid",ui->dxGridEntry->text());
  m_settings->setValue ("AstroDisplayed", m_astroWidget && m_astroWidget->isVisible());
  m_settings->setValue ("MsgAvgDisplayed", m_msgAvgWidget && m_msgAvgWidget->isVisible ());
  m_settings->setValue ("FoxLogDisplayed", m_foxLogWindow && m_foxLogWindow->isVisible ());
  m_settings->setValue ("ContestLogDisplayed", m_contestLogWindow && m_contestLogWindow->isVisible ());
  m_settings->setValue("CallFirst",ui->cbFirst->isChecked());
//SP6XD:
  m_settings->setValue("IgnoreRegex",ui->ignoreRegex->isChecked());
  m_settings->setValue("HoundSort",ui->comboBoxHoundSort->currentIndex());
  m_settings->setValue("FoxNlist",ui->sbNlist->value());
  m_settings->setValue("FoxNslots",ui->sbNslots->value());
  m_settings->setValue("FoxMaxDB_v2",ui->sbMax_dB->value()); // original key abandoned
  m_settings->setValue ("SerialNumber",ui->sbSerialNumber->value ());
  m_settings->endGroup();

  m_settings->beginGroup("Common");
  m_settings->setValue("Mode",m_mode);
  m_settings->setValue("SaveNone",ui->actionNone->isChecked());
  m_settings->setValue("SaveDecoded",ui->actionSave_decoded->isChecked());
  m_settings->setValue("SaveAll",ui->actionSave_all->isChecked());
  m_settings->setValue("NDepth",m_ndepth);
  m_settings->setValue("RxFreq",ui->RxFreqSpinBox->value());
  m_settings->setValue("TxFreq",ui->TxFreqSpinBox->value());
  m_settings->setValue("WSPRfreq",ui->WSPRfreqSpinBox->value());
  m_settings->setValue("FST4W_RxFreq",ui->sbFST4W_RxFreq->value());
  m_settings->setValue("FST4W_FTol",ui->sbFST4W_FTol->value());
  m_settings->setValue("FST4_FLow",ui->sbF_Low->value());
  m_settings->setValue("FST4_FHigh",ui->sbF_High->value());
  m_settings->setValue("SubMode",ui->sbSubmode->value());
  m_settings->setValue("DTtol",m_DTtol);
  m_settings->setValue("Ftol", ui->sbFtol->value ());
  m_settings->setValue("MinSync",m_minSync);
  m_settings->setValue ("AutoSeq", ui->cbAutoSeq->isChecked ());
  m_settings->setValue ("RxAll", ui->cbRxAll->isChecked ());
  m_settings->setValue("ShMsgs",m_bShMsgs);
  m_settings->setValue("SWL",ui->cbSWL->isChecked());
  m_settings->setValue ("DialFreq", QVariant::fromValue(m_lastMonitoredFrequency));
  m_settings->setValue("OutAttenuation", ui->outAttenuation->value ());
  m_settings->setValue("NoSuffix",m_noSuffix);
  m_settings->setValue("GUItab",ui->tabWidget->currentIndex());
  m_settings->setValue("OutBufSize",outBufSize);
  m_settings->setValue ("HoldTxFreq", ui->cbHoldTxFreq->isChecked ());
  m_settings->setValue("PctTx", ui->sbTxPercent->value ());
  m_settings->setValue("RoundRobin",ui->RoundRobin->currentText());
  m_settings->setValue("dBm",m_dBm);
  m_settings->setValue("RR73",m_send_RR73);
  m_settings->setValue ("WSPRPreferType1", ui->WSPR_prefer_type_1_check_box->isChecked ());
  m_settings->setValue("UploadSpots",m_uploadWSPRSpots);
  m_settings->setValue("NoOwnCall",ui->cbNoOwnCall->isChecked());
  m_settings->setValue ("BandHopping", ui->band_hopping_group_box->isChecked ());
  m_settings->setValue ("TRPeriod", ui->sbTR->value ());
  m_settings->setValue ("MaxDrift", ui->sbMaxDrift->value());
  m_settings->setValue ("TRPeriod_FST4W", ui->sbTR_FST4W->value ());
  m_settings->setValue("FastMode",m_bFastMode);
  m_settings->setValue("Fast9",m_bFast9);
  m_settings->setValue ("CQTxfreq", ui->sbCQTxFreq->value ());
  m_settings->setValue("pwrBandTxMemory",m_pwrBandTxMemory);
  m_settings->setValue("pwrBandTuneMemory",m_pwrBandTuneMemory);
  m_settings->setValue ("FT8AP", ui->actionEnable_AP_FT8->isChecked ());
  m_settings->setValue ("JT65AP", ui->actionEnable_AP_JT65->isChecked ());
  m_settings->setValue ("AutoClearAvg", ui->actionAuto_Clear_Avg->isChecked ());
  m_settings->setValue("SplitterState",ui->decodes_splitter->saveState());
  m_settings->setValue("Blanker",ui->sbNB->value());

  {
    QList<QVariant> coeffs;     // suitable for QSettings
    for (auto const& coeff : m_phaseEqCoefficients)
      {
        coeffs << coeff;
      }
    m_settings->setValue ("PhaseEqualizationCoefficients", QVariant {coeffs});
  }
  m_settings->endGroup();
}

//---------------------------------------------------------- readSettings()
void MainWindow::readSettings()
{
  ui->cbAutoSeq->setVisible(false);
  ui->cbFirst->setVisible(false);
  m_settings->beginGroup("MainWindow");
  std::array<QByteArray, 3> the_geometries;
  the_geometries[0] = m_settings->value ("geometry", saveGeometry ()).toByteArray ();
  the_geometries[1] = m_settings->value ("SWLModeGeometry", saveGeometry ()).toByteArray ();
  the_geometries[2] = m_settings->value ("geometryNoControls", saveGeometry ()).toByteArray ();
  auto SWL_mode = m_settings->value ("SWLView", false).toBool ();
  auto show_menus = m_settings->value ("ShowMenus", true).toBool ();
  ui->actionSWL_Mode->setChecked (SWL_mode);
  ui->cbMenus->setChecked (show_menus);
  auto current_view_mode = SWL_mode ? 1 : show_menus ? 0 : 2;
  change_layout (current_view_mode);
  geometries (current_view_mode, the_geometries);
  restoreState (m_settings->value ("state", saveState ()).toByteArray ());
  ui->dxCallEntry->setText (m_settings->value ("DXcall", QString {}).toString ());
  ui->dxGridEntry->setText (m_settings->value ("DXgrid", QString {}).toString ());
  m_path = m_settings->value("MRUdir", m_config.save_directory ().absolutePath ()).toString ();
  m_txFirst = m_settings->value("TxFirst",false).toBool();
  auto displayAstro = m_settings->value ("AstroDisplayed", false).toBool ();
  auto displayMsgAvg = m_settings->value ("MsgAvgDisplayed", false).toBool ();
  auto displayFoxLog = m_settings->value ("FoxLogDisplayed", false).toBool ();
  auto displayContestLog = m_settings->value ("ContestLogDisplayed", false).toBool ();
  ui->cbFirst->setChecked(m_settings->value("CallFirst",true).toBool());
//SP6XD:
  ui->ignoreRegex->setChecked(m_settings->value("IgnoreRegex",true).toBool());
  ui->comboBoxHoundSort->setCurrentIndex(m_settings->value("HoundSort",3).toInt());
  ui->sbNlist->setValue(m_settings->value("FoxNlist",12).toInt());
  m_Nslots=m_settings->value("FoxNslots",5).toInt();
  ui->sbNslots->setValue(m_Nslots);
  ui->sbMax_dB->setValue(m_settings->value("FoxMaxDB_v2",70).toInt());
  ui->sbSerialNumber->setValue (m_settings->value ("SerialNumber", 1).toInt ());
  m_settings->endGroup();

  // do this outside of settings group because it uses groups internally
  ui->actionAstronomical_data->setChecked (displayAstro);

  m_settings->beginGroup("Common");
  m_mode=m_settings->value("Mode","JT9").toString();
  ui->actionNone->setChecked(m_settings->value("SaveNone",true).toBool());
  ui->actionSave_decoded->setChecked(m_settings->value("SaveDecoded",false).toBool());
  ui->actionSave_all->setChecked(m_settings->value("SaveAll",false).toBool());
  ui->RxFreqSpinBox->setValue(0); // ensure a change is signaled
  ui->RxFreqSpinBox->setValue(m_settings->value("RxFreq",1500).toInt());
  ui->sbFST4W_RxFreq->setValue(0);
  ui->sbFST4W_RxFreq->setValue(m_settings->value("FST4W_RxFreq",1500).toInt());
  ui->sbF_Low->setValue(m_settings->value("FST4_FLow",600).toInt());
  ui->sbF_High->setValue(m_settings->value("FST4_FHigh",1400).toInt());
  m_nSubMode=m_settings->value("SubMode",0).toInt();
  ui->sbSubmode->setValue(m_nSubMode);
  ui->sbFtol->setValue (m_settings->value("Ftol", 50).toInt());
  ui->sbFST4W_FTol->setValue(m_settings->value("FST4W_FTol",100).toInt());
  m_minSync=m_settings->value("MinSync",0).toInt();
  ui->syncSpinBox->setValue(m_minSync);
  ui->cbAutoSeq->setChecked (m_settings->value ("AutoSeq", false).toBool());
  ui->cbRxAll->setChecked (m_settings->value ("RxAll", false).toBool());
  m_bShMsgs=m_settings->value("ShMsgs",false).toBool();
  m_bSWL=m_settings->value("SWL",false).toBool();
  m_bFast9=m_settings->value("Fast9",false).toBool();
  m_bFastMode=m_settings->value("FastMode",false).toBool();
  ui->sbTR->setValue (m_settings->value ("TRPeriod", 15).toInt());
  ui->sbMaxDrift->setValue (m_settings->value ("MaxDrift",0).toInt());
  ui->sbTR_FST4W->setValue (m_settings->value ("TRPeriod_FST4W", 15).toInt());
  m_lastMonitoredFrequency = m_settings->value ("DialFreq",
    QVariant::fromValue<Frequency> (default_frequency)).value<Frequency> ();
  ui->WSPRfreqSpinBox->setValue(0); // ensure a change is signaled
  ui->WSPRfreqSpinBox->setValue(m_settings->value("WSPRfreq",1500).toInt());
  ui->TxFreqSpinBox->setValue(0); // ensure a change is signaled
  ui->TxFreqSpinBox->setValue(m_settings->value("TxFreq",1500).toInt());
  m_ndepth=m_settings->value("NDepth",3).toInt();
  ui->sbTxPercent->setValue (m_settings->value ("PctTx", 20).toInt ());
  on_sbTxPercent_valueChanged (ui->sbTxPercent->value ());
  ui->RoundRobin->setCurrentText(m_settings->value("RoundRobin",tr("Random")).toString());
  m_dBm=m_settings->value("dBm",37).toInt();
  m_send_RR73=m_settings->value("RR73",false).toBool();
  if(m_send_RR73) {
    m_send_RR73=false;
    on_txrb4_doubleClicked();
  }
  ui->WSPR_prefer_type_1_check_box->setChecked (m_settings->value ("WSPRPreferType1", true).toBool ());
  m_uploadWSPRSpots=m_settings->value("UploadSpots",false).toBool();
  ui->cbNoOwnCall->setChecked(m_settings->value("NoOwnCall",false).toBool());
  ui->band_hopping_group_box->setChecked (m_settings->value ("BandHopping", false).toBool());
  // setup initial value of tx attenuator
  m_block_pwr_tooltip = true;
  ui->outAttenuation->setValue (m_settings->value ("OutAttenuation", 0).toInt ());
  m_block_pwr_tooltip = false;
  ui->sbCQTxFreq->setValue (m_settings->value ("CQTxFreq", 260).toInt());
  m_noSuffix=m_settings->value("NoSuffix",false).toBool();
  int n=m_settings->value("GUItab",0).toInt();
  ui->tabWidget->setCurrentIndex(n);
  outBufSize=m_settings->value("OutBufSize",4096).toInt();
  ui->cbHoldTxFreq->setChecked (m_settings->value ("HoldTxFreq", false).toBool ());
  m_pwrBandTxMemory=m_settings->value("pwrBandTxMemory").toHash();
  m_pwrBandTuneMemory=m_settings->value("pwrBandTuneMemory").toHash();
  ui->actionEnable_AP_FT8->setChecked (m_settings->value ("FT8AP", false).toBool());
  ui->actionEnable_AP_JT65->setChecked (m_settings->value ("JT65AP", false).toBool());
  ui->actionAuto_Clear_Avg->setChecked (m_settings->value ("AutoClearAvg", false).toBool());
  ui->decodes_splitter->restoreState(m_settings->value("SplitterState").toByteArray());
  ui->sbNB->setValue(m_settings->value("Blanker",0).toInt());
  {
    auto const& coeffs = m_settings->value ("PhaseEqualizationCoefficients"
                                            , QList<QVariant> {0., 0., 0., 0., 0.}).toList ();
    m_phaseEqCoefficients.clear ();
    for (auto const& coeff : coeffs)
      {
        m_phaseEqCoefficients.append (coeff.value<double> ());
      }
  }
  m_settings->endGroup();

  // use these initialisation settings to tune the audio o/p buffer
  // size and audio thread priority
  m_settings->beginGroup ("Tune");
  m_audioThreadPriority = static_cast<QThread::Priority> (m_settings->value ("Audio/ThreadPriority", QThread::HighPriority).toInt () % 8);
  m_settings->endGroup ();

  checkMSK144ContestType();
  if(displayMsgAvg) on_actionMessage_averaging_triggered();
  if (displayFoxLog) on_fox_log_action_triggered ();
  if (displayContestLog) on_contest_log_action_triggered ();
}

void MainWindow::checkMSK144ContestType()
{
  if(SpecOp::NONE != m_config.special_op_id()) 
    {
      if(m_mode=="MSK144" && SpecOp::EU_VHF < m_config.special_op_id())
        {
          MessageBox::warning_message (this, tr ("Improper mode"),
           "Mode will be changed to FT8. MSK144 not available if Field Day, WW Digi, RTTY or Fox/Hound is selected.");
          on_actionFT8_triggered();
        }
    }
}

void MainWindow::set_application_font (QFont const& font)
{
  qApp->setFont (font);
  // set font in the application style sheet as well in case it has
  // been modified in the style sheet which has priority
  QString ss;
  if (qApp->styleSheet ().size ())
    {
      auto sheet = qApp->styleSheet ();
      sheet.remove ("file:///");
      QFile sf {sheet};
      if (sf.open (QFile::ReadOnly | QFile::Text))
        {
          ss = sf.readAll () + ss;
        }
    }
  qApp->setStyleSheet (ss + "* {" + font_as_stylesheet (font) + '}');
  for (auto& widget : qApp->topLevelWidgets ())
    {
      widget->updateGeometry ();
    }
}

void MainWindow::setDecodedTextFont (QFont const& font)
{
  ui->decodedTextBrowser->setContentFont (font);
  ui->decodedTextBrowser2->setContentFont (font);
  ui->textBrowser4->setContentFont(font);
  ui->textBrowser4->displayFoxToBeCalled(" ");
  ui->textBrowser4->setText("");
  auto style_sheet = "QLabel {" + font_as_stylesheet (font) + '}';
  ui->lh_decodes_headings_label->setStyleSheet (ui->lh_decodes_headings_label->styleSheet () + style_sheet);
  ui->rh_decodes_headings_label->setStyleSheet (ui->rh_decodes_headings_label->styleSheet () + style_sheet);
  if (m_msgAvgWidget) {
    m_msgAvgWidget->changeFont (font);
  }
  if (m_foxLogWindow) {
    m_foxLogWindow->set_log_view_font (font);
  }
  if (m_contestLogWindow) {
    m_contestLogWindow->set_log_view_font (font);
  }
  updateGeometry ();
}

void MainWindow::fixStop()
{
  m_hsymStop=179;
  if(m_mode=="WSPR") {
    m_hsymStop=396;
  } else if(m_mode=="Echo") {
    m_hsymStop=9;
  } else if (m_mode=="JT4"){
    m_hsymStop=176;
    if(m_config.decode_at_52s()) m_hsymStop=179;
  } else if (m_mode=="JT9"){
    m_hsymStop=173;
    if(m_config.decode_at_52s()) m_hsymStop=179;
  } else if (m_mode=="JT65"){
    m_hsymStop=174;
    if(m_config.decode_at_52s()) m_hsymStop=179;
  } else if (m_mode=="Q65"){
    m_hsymStop=48;                                  // 13.8 s
    if(m_TRperiod==30) {
      m_hsymStop=96;                                // 27.6 s
      if(m_config.decode_at_52s()) m_hsymStop=100;  // 28.8 s
    }
    if(m_TRperiod==60) m_hsymStop=196;              // 56.4 s
    if(m_TRperiod==120) m_hsymStop=408;             // 117.5 s
    if(m_TRperiod==300) m_hsymStop=1030;            // 296.6 s
  } else if (m_mode=="FreqCal"){
    m_hsymStop=((int(m_TRperiod/0.288))/8)*8;
  } else if (m_mode=="FT8") {
    m_hsymStop=50;
  } else if (m_mode=="FT4") {
  m_hsymStop=21;
  } else if(m_mode=="FST4" or m_mode=="FST4W") {
    int stop[] = {39,85,187,387,1003,3107,6232};
    int stop_EME[] = {48,95,197,396,1012,3107,6232};
    int i=0;
    if(m_TRperiod==30) i=1;
    if(m_TRperiod==60) i=2;
    if(m_TRperiod==120) i=3;
    if(m_TRperiod==300) i=4;
    if(m_TRperiod==900) i=5;
    if(m_TRperiod==1800) i=6;
    if(m_config.decode_at_52s()) {
      m_hsymStop=stop_EME[i];
    } else {
      m_hsymStop=stop[i];
    }
  }
}

//-------------------------------------------------------------- dataSink()
void MainWindow::dataSink(qint64 frames)
{
  static float s[NSMAX];
  char line[80];
  int k(frames);
  auto fname {QDir::toNativeSeparators(m_config.writeable_data_dir ().absoluteFilePath ("refspec.dat")).toLocal8Bit ()};

  if(m_diskData) {
    dec_data.params.ndiskdat=1;
  } else {
    dec_data.params.ndiskdat=0;
    m_wideGraph->setDiskUTC(-1);
  }

  m_bUseRef=m_wideGraph->useRef();
  if(!m_diskData) {
    refspectrum_(&dec_data.d2[k-m_nsps/2],&m_bClearRefSpec,&m_bRefSpec,
                 &m_bUseRef, fname.constData (), fname.size ());
  }
  m_bClearRefSpec=false;

  if(m_mode=="MSK144" or m_bFast9) {
    fastSink(frames);
    if(m_bFastMode) return;
  }

// Get power, spectrum, and ihsym
  dec_data.params.nfa=m_wideGraph->nStartFreq();
  dec_data.params.nfb=m_wideGraph->Fmax();
  if(m_mode=="FST4") {
    dec_data.params.nfa=ui->sbF_Low->value();
    dec_data.params.nfb=ui->sbF_High->value();
  }
  int nsps=m_nsps;
  if(m_bFastMode) nsps=6912;
  int nsmo=m_wideGraph->smoothYellow()-1;
  bool bLowSidelobes=m_config.lowSidelobes();
  int npct=0;
  if(m_mode.startsWith("FST4")) npct=ui->sbNB->value();
  symspec_(&dec_data,&k,&m_TRperiod,&nsps,&m_inGain,&bLowSidelobes,&nsmo,&m_px,s,
           &m_df3,&m_ihsym,&m_npts8,&m_pxmax,&npct);
  if(m_mode=="WSPR" or m_mode=="FST4W") wspr_downsample_(dec_data.d2,&k);
  if(m_ihsym <=0) return;
  if(ui) ui->signal_meter_widget->setValue(m_px,m_pxmax); // Update thermometer
  if(m_monitoring || m_diskData) {
    m_wideGraph->dataSink2(s,m_df3,m_ihsym,m_diskData);
  }
  if(m_mode=="MSK144") return;

  fixStop();
  if (m_mode == "FreqCal"
      // only calculate after 1st chunk, also skip chunk where rig
      // changed frequency
      && !(m_ihsym % 8) && m_ihsym > 8 && m_ihsym <= m_hsymStop) {
    int RxFreq=ui->RxFreqSpinBox->value ();
    int nkhz=(m_freqNominal+RxFreq)/1000;
    int ftol = ui->sbFtol->value ();
    freqcal_(&dec_data.d2[0],&k,&nkhz,&RxFreq,&ftol,&line[0],80);
    QString t=QString::fromLatin1(line);
    DecodedText decodedtext {t};
    ui->decodedTextBrowser->displayDecodedText (decodedtext, m_config.my_callsign (), m_mode, m_config.DXCC (),
          m_logBook, m_currentBand, m_config.ppfx ());
    if (ui->measure_check_box->isChecked ()) {
      // Append results text to file "fmt.all".
      QFile f {m_config.writeable_data_dir ().absoluteFilePath ("fmt.all")};
      if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&f);
        out << t
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
            << Qt::endl
#else
            << endl
#endif
          ;
        f.close();
      } else {
        MessageBox::warning_message (this, tr ("File Open Error")
                                     , tr ("Cannot open \"%1\" for append: %2")
                                     .arg (f.fileName ()).arg (f.errorString ()));
      }
    }
    if(m_ihsym==m_hsymStop && ui->actionFrequency_calibration->isChecked()) {
      freqCalStep();
    }
  }

  if(m_ihsym==3*m_hsymStop/4) {
    m_dialFreqRxWSPR=m_freqNominal;
  }

  if(m_mode=="FT8") {
    to_jt9(m_ihsym,-1,-1);     //Allow jt9 to bail out early, if necessary
    if(m_ihsym==40 and m_decoderBusy) {
      qDebug() << "Clearing hung decoder status";
      decodeDone();  //Clear a hung decoder status
    }
  }

  bool bCallDecoder=false;
  if(m_ihsym==m_hsymStop) bCallDecoder=true;
  if(m_mode=="FT8" and !m_diskData) {
    if(m_ihsym==m_earlyDecode) bCallDecoder=true;
    if(m_ihsym==m_earlyDecode2) bCallDecoder=true;
  }
  if(bCallDecoder) {
    if(m_mode=="Echo") {
      float snr=0;
      int nfrit=0;
      int nqual=0;
      float f1=1500.0;
      float xlevel=0.0;
      float sigdb=0.0;
      float dfreq=0.0;
      float width=0.0;
      echocom_.nclearave=m_nclearave;
      int nDop=0;
      avecho_(dec_data.d2,&nDop,&nfrit,&nqual,&f1,&xlevel,&sigdb,
          &snr,&dfreq,&width);
      QString t;
      t = t.asprintf("%3d %7.1f %7.1f %7.1f %7.1f %3d",echocom_.nsum,xlevel,sigdb,
                dfreq,width,nqual);
      t=QDateTime::currentDateTimeUtc().toString("hh:mm:ss  ") + t;
      if (ui) ui->decodedTextBrowser->appendText(t);
      if(m_echoGraph->isVisible()) m_echoGraph->plotSpec();
      m_nclearave=0;
//Don't restart Monitor after an Echo transmission
      if(m_bEchoTxed and !m_auto) {
        monitor(false);
        m_bEchoTxed=false;
      }
      return;
    }
    if(m_mode=="FreqCal") {
      return;
    }
    if( m_dialFreqRxWSPR==0) m_dialFreqRxWSPR=m_freqNominal;
    m_dataAvailable=true;
    dec_data.params.npts8=(m_ihsym*m_nsps)/16;
    dec_data.params.newdat=1;
    dec_data.params.nagain=0;
    dec_data.params.nzhsym=m_hsymStop;
    if(m_mode=="FT8" and m_ihsym==m_earlyDecode and !m_diskData) dec_data.params.nzhsym=m_earlyDecode;
    if(m_mode=="FT8" and m_ihsym==m_earlyDecode2 and !m_diskData) dec_data.params.nzhsym=m_earlyDecode2;
    QDateTime now {QDateTime::currentDateTimeUtc ()};
    m_dateTime = now.toString ("yyyy-MMM-dd hh:mm");
    if(m_mode!="WSPR") decode(); //Start decoder

    if(m_mode=="FT8" and !m_diskData and (m_ihsym==m_earlyDecode or m_ihsym==m_earlyDecode2)) return;
    if (!m_diskData)
      {
        Q_EMIT reset_audio_input_stream (true); // reports dropped samples
      }
    if(!m_diskData and (m_saveAll or m_saveDecoded or m_mode=="WSPR")) {
      //Always save unless "Save None"; may delete later
      if(m_TRperiod < 60) {
        int n=fmod(double(now.time().second()),m_TRperiod);
        if(n<(m_TRperiod/2)) n=n+m_TRperiod;
        auto const& period_start=now.addSecs(-n);
        m_fnameWE=m_config.save_directory().absoluteFilePath (period_start.toString("yyMMdd_hhmmss"));
      } else {
        auto const& period_start = now.addSecs (-(now.time ().minute () % (int(m_TRperiod) / 60)) * 60);
        m_fnameWE=m_config.save_directory ().absoluteFilePath (period_start.toString ("yyMMdd_hhmm"));
      }
      int samples=m_TRperiod*12000;
      if(m_mode=="FT4") samples=21*3456;

      // the following is potential a threading hazard - not a good
      // idea to pass pointer to be processed in another thread
      m_saveWAVWatcher.setFuture (QtConcurrent::run (std::bind (&MainWindow::save_wave_file,
            this, m_fnameWE, &dec_data.d2[0], samples, m_config.my_callsign(),
            m_config.my_grid(), m_mode, m_nSubMode, m_freqNominal, m_hisCall, m_hisGrid)));
      if (m_mode=="WSPR") {
        auto c2name {(m_fnameWE + ".c2").toLocal8Bit ()};
        int nsec=120;
        int nbfo=1500;
        double f0m1500=m_freqNominal/1000000.0 + nbfo - 1500;
        int err = savec2_(c2name.constData (),&nsec,&f0m1500, c2name.size ());
        if (err!=0) MessageBox::warning_message (this, tr ("Error saving c2 file"), c2name);
      }
    }
    if(m_mode=="WSPR") {
      QStringList t2;
      QStringList depth_args;
      t2 << "-f" << QString {"%1"}.arg (m_dialFreqRxWSPR / 1e6, 0, 'f', 6);
      if((m_ndepth&7)==1) depth_args << "-qB"; //2 pass w subtract, no Block detection, no shift jittering
      if((m_ndepth&7)==2) depth_args << "-C" << "500" << "-o" << "4"; //3 pass, subtract, Block detection, OSD 
      if((m_ndepth&7)==3) depth_args << "-C" << "500"  << "-o" << "4" << "-d"; //3 pass, subtract, Block detect, OSD, more candidates 
      QStringList degrade;
      degrade << "-d" << QString {"%1"}.arg (m_config.degrade(), 4, 'f', 1);
      m_cmndP1.clear ();
      if(m_diskData) {
        m_cmndP1 << depth_args << "-a"
                 << QDir::toNativeSeparators (m_config.writeable_data_dir ().absolutePath()) << m_path;
      } else {
        m_cmndP1 << depth_args << "-a"
                 << QDir::toNativeSeparators (m_config.writeable_data_dir ().absolutePath())
                 << t2 << m_fnameWE + ".wav";
      }
      if (ui) ui->DecodeButton->setChecked (true);
      p1Timer.start(1000);
      m_decoderBusy = true;
      statusUpdate ();
    }
    m_rxDone=true;
  }
}

void MainWindow::startP1()
{
  p1.start (QDir::toNativeSeparators (QDir {QApplication::applicationDirPath ()}.absoluteFilePath ("wsprd")), m_cmndP1);
}

QString MainWindow::save_wave_file (QString const& name, short const * data, int samples,
        QString const& my_callsign, QString const& my_grid, QString const& mode, qint32 sub_mode,
        Frequency frequency, QString const& his_call, QString const& his_grid) const
{
  //
  // This member function runs in a thread and should not access
  // members that may be changed in the GUI thread or any other thread
  // without suitable synchronization.
  //
  QAudioFormat format;
  format.setCodec ("audio/pcm");
  format.setSampleRate (12000);
  format.setChannelCount (1);
  format.setSampleSize (16);
  format.setSampleType (QAudioFormat::SignedInt);
  auto source = QString {"%1; %2"}.arg (my_callsign).arg (my_grid);
  auto comment = QString {"Mode=%1%2; Freq=%3%4"}
                   .arg (mode)
                   .arg (QString {(mode.contains ('J') && !mode.contains ('+'))
                         || mode.startsWith ("FST4") || mode.startsWith ('Q')
                         ? QString {"; Sub Mode="} + QString::number (int (samples / 12000)) + QChar {'A' + sub_mode}
                       : QString {}})
                   .arg (Radio::frequency_MHz_string (frequency))
                   .arg (QString {mode!="WSPR" ? QString {"; DXCall=%1; DXGrid=%2"}
         .arg (his_call)
         .arg (his_grid).toLocal8Bit () : ""});
  BWFFile::InfoDictionary list_info {
      {{{'I','S','R','C'}}, source.toLocal8Bit ()},
      {{{'I','S','F','T'}}, program_title (revision ()).simplified ().toLocal8Bit ()},
      {{{'I','C','R','D'}}, QDateTime::currentDateTimeUtc ()
                          .toString ("yyyy-MM-ddTHH:mm:ss.zzzZ").toLocal8Bit ()},
      {{{'I','C','M','T'}}, comment.toLocal8Bit ()},
        };
  auto file_name = name + ".wav";
  BWFFile wav {format, file_name, list_info};
  if (!wav.open (BWFFile::WriteOnly)
      || 0 > wav.write (reinterpret_cast<char const *> (data)
                        , sizeof (short) * samples))
    {
      return file_name + ": " + wav.errorString ();
    }
  return QString {};
}

//-------------------------------------------------------------- fastSink()
void MainWindow::fastSink(qint64 frames)
{
  int k (frames);
  bool decodeNow=false;
  if(k < m_k0) {                                 //New sequence ?
    memcpy(fast_green2,fast_green,4*703);        //Copy fast_green[] to fast_green2[]
    memcpy(fast_s2,fast_s,4*703*64);             //Copy fast_s[] into fast_s2[]
    fast_jh2=fast_jh;
    if(!m_diskData) memset(dec_data.d2,0,2*30*12000);   //Zero the d2[] array
    m_bFastDecodeCalled=false;
    m_bDecoded=false;
  }

  QDateTime tnow=QDateTime::currentDateTimeUtc();
  int ihr=tnow.toString("hh").toInt();
  int imin=tnow.toString("mm").toInt();
  int isec=tnow.toString("ss").toInt();
  isec=isec - fmod(double(isec),m_TRperiod);
  int nutc0=10000*ihr + 100*imin + isec;
  if(m_diskData) nutc0=m_UTCdisk;
  char line[80];
  bool bmsk144=((m_mode=="MSK144") and (m_monitoring or m_diskData));
  line[0]=0;

  int RxFreq=ui->RxFreqSpinBox->value ();
  int nTRpDepth=m_TRperiod + 1000*(m_ndepth & 3);
  qint64 ms0 = QDateTime::currentMSecsSinceEpoch();
//  ::memcpy(dec_data.params.mycall, (m_baseCall+"            ").toLatin1(),sizeof dec_data.params.mycall);
  ::memcpy(dec_data.params.mycall,(m_config.my_callsign () + "            ").toLatin1(),sizeof dec_data.params.mycall);
  QString hisCall {ui->dxCallEntry->text ()};
  bool bshmsg=ui->cbShMsgs->isChecked();
  bool bswl=ui->cbSWL->isChecked();
//  ::memcpy(dec_data.params.hiscall,(Radio::base_callsign (hisCall) +  "            ").toLatin1 ().constData (), sizeof dec_data.params.hiscall);
  ::memcpy(dec_data.params.hiscall,(hisCall + "            ").toLatin1 ().constData (), sizeof dec_data.params.hiscall);
  ::memcpy(dec_data.params.mygrid, (m_config.my_grid()+"      ").toLatin1(), sizeof dec_data.params.mygrid);
  auto data_dir {m_config.writeable_data_dir ().absolutePath ().toLocal8Bit ()};
  float pxmax = 0;
  float rmsNoGain = 0;
  int ftol = ui->sbFtol->value ();
  hspec_(dec_data.d2,&k,&nutc0,&nTRpDepth,&RxFreq,&ftol,&bmsk144,
         &m_bTrain,m_phaseEqCoefficients.constData(),&m_inGain,&dec_data.params.mycall[0],
         &dec_data.params.hiscall[0],&bshmsg,&bswl,
         data_dir.constData (),fast_green,fast_s,&fast_jh,&pxmax,&rmsNoGain,&line[0],12,12,data_dir.size (),80);
  float px = fast_green[fast_jh];
  QString t;
  t = t.asprintf(" Rx noise: %5.1f ",px);
  ui->signal_meter_widget->setValue(rmsNoGain,pxmax); // Update thermometer
  m_fastGraph->plotSpec(m_diskData,m_UTCdisk);



  if(bmsk144 and (line[0]!=0)) {
    QString message {QString::fromLatin1 (line)};
    DecodedText decodedtext {message.replace (QChar::LineFeed, "")};
    ui->decodedTextBrowser->displayDecodedText (decodedtext, m_config.my_callsign (), m_mode, m_config.DXCC(),
         m_logBook, m_currentBand, m_config.ppfx ());
    m_bDecoded=true;
    auto_sequence (decodedtext, ui->sbFtol->value (), std::numeric_limits<unsigned>::max ());
    postDecode (true, decodedtext.string ());
//    writeAllTxt(message);
    write_all("Rx",message);
    bool stdMsg = decodedtext.report(m_baseCall,
                  Radio::base_callsign(ui->dxCallEntry->text()),m_rptRcvd);
    if (stdMsg) pskPost (decodedtext);
  }

  float fracTR=float(k)/(12000.0*m_TRperiod);
  decodeNow=false;
  if(fracTR>0.92) {
    m_dataAvailable=true;
    fast_decode_done();
    m_bFastDone=true;
  }

  m_k0=k;
  if(m_diskData and m_k0 >= dec_data.params.kin - 7 * 512) decodeNow=true;
  if(!m_diskData and m_tRemaining<0.35 and !m_bFastDecodeCalled) decodeNow=true;
  if(m_mode=="MSK144") decodeNow=false;

  if(decodeNow) {
    m_dataAvailable=true;
    m_t0=0.0;
    m_t1=k/12000.0;
    m_kdone=k;
    dec_data.params.newdat=1;
    if(!m_decoderBusy) {
      m_bFastDecodeCalled=true;
      decode();
    }
  }

  if(decodeNow or m_bFastDone) {
    if(!m_diskData and (m_saveAll or m_saveDecoded)) {
      QDateTime now {QDateTime::currentDateTimeUtc()};
      int n=fmod(double(now.time().second()),m_TRperiod);
      if(n<(m_TRperiod/2)) n=n+m_TRperiod;
      auto const& period_start = now.addSecs (-n);
      m_fnameWE = m_config.save_directory ().absoluteFilePath (period_start.toString ("yyMMdd_hhmmss"));
      if(m_saveAll or m_bAltV or (m_bDecoded and m_saveDecoded) or (m_mode!="MSK144")) {
        m_bAltV=false;
        // the following is potential a threading hazard - not a good
        // idea to pass pointer to be processed in another thread
        m_saveWAVWatcher.setFuture (QtConcurrent::run (std::bind (&MainWindow::save_wave_file,
           this, m_fnameWE, &dec_data.d2[0], int(m_TRperiod*12000.0), m_config.my_callsign(),
           m_config.my_grid(), m_mode, m_nSubMode, m_freqNominal, m_hisCall, m_hisGrid)));
      }
      if(m_mode!="MSK144") {
        killFileTimer.start (int(750.0*m_TRperiod)); //Kill 3/4 period from now
      }
    }
    m_bFastDone=false;
  }
  float tsec=0.001*(QDateTime::currentMSecsSinceEpoch() - ms0);
  m_fCPUmskrtd=0.9*m_fCPUmskrtd + 0.1*tsec;
}

void MainWindow::showSoundInError(const QString& errorMsg)
{
  if (m_splash && m_splash->isVisible ()) m_splash->hide ();
  MessageBox::critical_message (this, tr ("Error in Sound Input"), errorMsg);
}

void MainWindow::showSoundOutError(const QString& errorMsg)
{
  if (m_splash && m_splash->isVisible ()) m_splash->hide ();
  MessageBox::critical_message (this, tr ("Error in Sound Output"), errorMsg);
}

void MainWindow::showStatusMessage(const QString& statusMsg)
{
  statusBar()->showMessage(statusMsg, 5000);
}

void MainWindow::on_actionSettings_triggered()               //Setup Dialog
{
  // things that might change that we need know about
  auto callsign = m_config.my_callsign ();
  auto my_grid = m_config.my_grid ();
  SpecOp nContest0=m_config.special_op_id();
  auto psk_on = m_config.spot_to_psk_reporter ();
  if (QDialog::Accepted == m_config.exec ()) {
    checkMSK144ContestType();
    if (m_config.my_callsign () != callsign) {
      m_baseCall = Radio::base_callsign (m_config.my_callsign ());
      ui->tx1->setEnabled (elide_tx1_not_allowed () || ui->tx1->isEnabled ());
      morse_(const_cast<char *> (m_config.my_callsign ().toLatin1().constData()),
             const_cast<int *> (icw), &m_ncw, m_config.my_callsign ().length());
    }
    if (m_config.my_callsign () != callsign || m_config.my_grid () != my_grid) {
      statusUpdate ();
    }
    on_dxGridEntry_textChanged (m_hisGrid); // recalculate distances in case of units change
    enable_DXCC_entity (m_config.DXCC ());  // sets text window proportions and (re)inits the logbook

    pskSetLocal ();
    // this will close the connection to PSKReporter if it has been
    // disabled
    if (psk_on && !m_config.spot_to_psk_reporter ())
      {
        m_psk_Reporter.sendReport (true);
      }

    if(m_config.restart_audio_input () && !m_config.audio_input_device ().isNull ()) {
      Q_EMIT startAudioInputStream (m_config.audio_input_device ()
                                    , m_rx_audio_buffer_frames
                                    , m_detector, m_downSampleFactor
                                    , m_config.audio_input_channel ());
    }

    if(m_config.restart_audio_output () && !m_config.audio_output_device ().isNull ()) {
      Q_EMIT initializeAudioOutputStream (m_config.audio_output_device ()
                                          , AudioDevice::Mono == m_config.audio_output_channel () ? 1 : 2
                                          , m_tx_audio_buffer_frames);
    }

    displayDialFrequency ();
    bool vhf {m_config.enable_VHF_features()};
    m_wideGraph->setVHF(vhf);
    if (!vhf) ui->sbSubmode->setValue (0);

    setup_status_bar (vhf);
    bool b = vhf && (m_mode=="JT4" or m_mode=="JT65" or
                     m_mode=="JT9" or m_mode=="MSK144" or m_mode=="Q65");
    if(b) VHF_features_enabled(b);
    set_mode (m_mode);
    if(b) VHF_features_enabled(b);

    m_config.transceiver_online ();
    if(!m_bFastMode) setXIT (ui->TxFreqSpinBox->value ());
    if ((m_config.single_decode () && !m_mode.startsWith ("FST4")) || m_mode=="JT4") {
      ui->lh_decodes_title_label->setText(tr ("Single-Period Decodes"));
      ui->rh_decodes_title_label->setText(tr ("Average Decodes"));
    }

    update_watchdog_label ();
    if(!m_splitMode) ui->cbCQTx->setChecked(false);
    if(!m_config.enable_VHF_features()) {
      ui->actionInclude_averaging->setVisible(false);
      ui->actionInclude_correlation->setVisible (false);
      ui->actionInclude_averaging->setChecked(false);
      ui->actionInclude_correlation->setChecked(false);
      ui->actionEnable_AP_JT65->setVisible(false);
      ui->actionAuto_Clear_Avg->setVisible(false);
    }
    if(m_config.special_op_id()!=nContest0) {
      ui->tx1->setEnabled(true);
      ui->txb1->setEnabled(true);
    }
    chkFT4();
    if(SpecOp::EU_VHF==m_config.special_op_id() and m_config.my_grid().size()<6) {
      MessageBox::information_message (this,
          "EU VHF Contest messages require a 6-character locator.");
    }
    if((m_config.special_op_id()==SpecOp::FOX or m_config.special_op_id()==SpecOp::HOUND) and
       m_mode!="FT8") {
      MessageBox::information_message (this,
          "Fox-and-Hound operation is available only in FT8 mode.\nGo back and change your selection.");
    }
  }
}

void MainWindow::on_monitorButton_clicked (bool checked)
{
  if (!m_transmitting) {
    auto prior = m_monitoring;
    monitor (checked);
    if (checked && !prior) {
      if (m_config.monitor_last_used ()) {
              // put rig back where it was when last in control
        setRig (m_lastMonitoredFrequency);
        setXIT (ui->TxFreqSpinBox->value ());
      }
          // ensure FreqCal triggers
      if(m_mode=="FST4W") {
        on_sbFST4W_RxFreq_valueChanged(ui->sbFST4W_RxFreq->value());
      } else {
        on_RxFreqSpinBox_valueChanged (ui->RxFreqSpinBox->value ());
      }
    }
      //Get Configuration in/out of strict split and mode checking
    m_config.sync_transceiver (true, checked);
  } else {
    ui->monitorButton->setChecked (false); // disallow
  }
}

void MainWindow::monitor (bool state)
{
  ui->monitorButton->setChecked (state);
  if (state) {
    m_diskData = false; // no longer reading WAV files
    if (!m_monitoring) Q_EMIT resumeAudioInputStream ();
  } else {
    Q_EMIT suspendAudioInputStream ();
  }
  m_monitoring = state;
}

void MainWindow::on_actionAbout_triggered()                  //Display "About"
{
  CAboutDlg {this}.exec ();
}

void MainWindow::on_autoButton_clicked (bool checked)
{
  m_auto = checked;
  if (checked
      && ui->cbFirst->isVisible () && ui->cbFirst->isChecked()
      && CALLING == m_QSOProgress) {
    m_bAutoReply = false;         // ready for next
    m_bCallingCQ = true;        // allows tail-enders to be picked up
    ui->cbFirst->setStyleSheet ("QCheckBox{color:red}");
  } else {
    ui->cbFirst->setStyleSheet("");
  }
  if (!checked) m_bCallingCQ = false;
  statusUpdate ();
  m_bEchoTxOK=false;
  if(m_auto and (m_mode=="Echo")) {
    m_nclearave=1;
    echocom_.nsum=0;
  }
  m_tAutoOn=QDateTime::currentMSecsSinceEpoch()/1000;
}

void MainWindow::on_sbTxPercent_valueChanged (int n)
{
  update_dynamic_property (ui->sbTxPercent, "notx", !n);
}

void MainWindow::auto_tx_mode (bool state)
{
  ui->autoButton->setChecked (state);
  on_autoButton_clicked (state);
}

void MainWindow::keyPressEvent (QKeyEvent * e)
{

  if(SpecOp::FOX == m_config.special_op_id()) {
    switch (e->key()) {
      case Qt::Key_Return:
        doubleClickOnCall2(Qt::KeyboardModifier(Qt::ShiftModifier + Qt::ControlModifier + Qt::AltModifier));
        return;
      case Qt::Key_Enter:
        doubleClickOnCall2(Qt::KeyboardModifier(Qt::ShiftModifier + Qt::ControlModifier + Qt::AltModifier));
        return;
      case Qt::Key_Backspace:
        qDebug() << "Key Backspace";
        return;
    }
    QMainWindow::keyPressEvent (e);
  }

  if(SpecOp::HOUND == m_config.special_op_id()) {
    switch (e->key()) {
      case Qt::Key_Return:
        auto_tx_mode(true);
        return;
      case Qt::Key_Enter:
        auto_tx_mode(true);
        return;
    }
    QMainWindow::keyPressEvent (e);
  }

  int n;
  bool bAltF1F6=m_config.alternate_bindings();
  switch(e->key())
    {
  case Qt::Key_B:
    if(m_mode=="FT4" && e->modifiers() & Qt::AltModifier) {
      on_pbBestSP_clicked();
    }
  return;
    case Qt::Key_C:
      if(m_mode=="FT4" && e->modifiers() & Qt::AltModifier) {
        bool b=ui->cbFirst->isChecked();
        ui->cbFirst->setChecked(!b);
      }
    return;
    case Qt::Key_D:
      if(m_mode != "WSPR" && e->modifiers() & Qt::ShiftModifier) {
        if(!m_decoderBusy) {
          dec_data.params.newdat=0;
          dec_data.params.nagain=0;
          decode();
          return;
        }
      }
      break;
    case Qt::Key_F1:
      if(bAltF1F6) {
        auto_tx_mode(true);
        on_txb6_clicked();
        return;
      } else {
        on_actionOnline_User_Guide_triggered();
        return;
      }
    case Qt::Key_F2:
      if(bAltF1F6) {
        auto_tx_mode(true);
        on_txb2_clicked();
        return;
      } else {
        on_actionSettings_triggered();
        return;
      }
    case Qt::Key_F3:
      if(bAltF1F6) {
        auto_tx_mode(true);
        on_txb3_clicked();
        return;
      } else {
        on_actionKeyboard_shortcuts_triggered();
        return;
      }
    case Qt::Key_F4:
      if(bAltF1F6) {
        auto_tx_mode(true);
        on_txb4_clicked();
        return;
      } else {
        clearDX ();
        ui->dxCallEntry->setFocus();
        return;
      }
    case Qt::Key_F5:
      if(bAltF1F6) {
        auto_tx_mode(true);
        on_txb5_clicked();
        return;
      } else {
        on_actionSpecial_mouse_commands_triggered();
        return;
      }
    case Qt::Key_F6:
      if(bAltF1F6) {
        bool b=ui->cbFirst->isChecked();
        ui->cbFirst->setChecked(!b);
      } else {
        if(e->modifiers() & Qt::ShiftModifier) {
          on_actionDecode_remaining_files_in_directory_triggered();
        } else {
          on_actionOpen_next_in_directory_triggered();
        }
      }
      return;
    case Qt::Key_F11:
      if((e->modifiers() & Qt::ControlModifier) and (e->modifiers() & Qt::ShiftModifier)) {
        m_bandEdited = true;
        band_changed(m_freqNominal-2000);
      } else {
        n=11;
        if(e->modifiers() & Qt::ControlModifier) n+=100;
        if(e->modifiers() & Qt::ShiftModifier) {
          int offset=60;
          if(m_mode=="FT4") offset=90;
          ui->TxFreqSpinBox->setValue(ui->TxFreqSpinBox->value()-offset);
        } else{
          bumpFqso(n);
        }
      }
      return;
    case Qt::Key_F12:
      if((e->modifiers() & Qt::ControlModifier) and (e->modifiers() & Qt::ShiftModifier)) {
        m_bandEdited = true;
        band_changed(m_freqNominal+2000);
      } else {
        n=12;
        if(e->modifiers() & Qt::ControlModifier) n+=100;
        if(e->modifiers() & Qt::ShiftModifier) {
          int offset=60;
          if(m_mode=="FT4") offset=90;
          ui->TxFreqSpinBox->setValue(ui->TxFreqSpinBox->value()+offset);
        } else {
          bumpFqso(n);
        }
      }
      return;
    case Qt::Key_Escape:
      m_nextCall="";
      on_stopTxButton_clicked();
      abortQSO();
      return;
    case Qt::Key_E:
      if((e->modifiers() & Qt::ShiftModifier) and SpecOp::FOX > m_config.special_op_id()) {
          ui->txFirstCheckBox->setChecked(false);
          return;
      }
      else if((e->modifiers() & Qt::ControlModifier) and SpecOp::FOX > m_config.special_op_id()) {
          ui->txFirstCheckBox->setChecked(true);
          return;
      }
      break;
    case Qt::Key_F:
      if(e->modifiers() & Qt::ControlModifier) {
        if(ui->tabWidget->currentIndex()==0) {
          ui->tx5->clearEditText();
          ui->tx5->setFocus();
        }
        return;
      }
      break;
    case Qt::Key_G:
      if(e->modifiers() & Qt::AltModifier) {
        genStdMsgs (m_rpt, true);
        return;
      }
      break;
    case Qt::Key_H:
      if(e->modifiers() & Qt::AltModifier) {
        on_stopTxButton_clicked();
        return;
      }
      break;
    case Qt::Key_L:
      if(e->modifiers() & Qt::ControlModifier) {
        lookup();
        genStdMsgs(m_rpt);
        return;
      }
      break;
    case Qt::Key_O:
      if(e->modifiers() & Qt::ControlModifier) {
          on_actionOpen_triggered();
          return;
      }
      else if(e->modifiers() & Qt::AltModifier) {
        bool ok;
        auto call = QInputDialog::getText (this, tr ("Change Operator"), tr ("New operator:"),
                                           QLineEdit::Normal, m_config.opCall (), &ok);
        if (ok) {
          m_config.opCall (call);
        }
        return;
      }
      break;
  case Qt::Key_R:
    if(e->modifiers() & Qt::AltModifier) {
      if(!m_send_RR73) on_txrb4_doubleClicked();
    return;
    }
    if(e->modifiers() & Qt::ControlModifier) {
      if(m_send_RR73) on_txrb4_doubleClicked();
      return;
    }
    break;
  case Qt::Key_X:
    if(e->modifiers() & Qt::AltModifier) {
//      qDebug() << "Alt-X" << m_mode << m_TRperiod << m_nsps << m_bFast9
//               << tx_duration(m_mode,m_TRperiod,m_nsps,m_bFast9);
      return;
    }
  }

  QMainWindow::keyPressEvent (e);
}

void MainWindow::bumpFqso(int n)                                 //bumpFqso()
{
  int i;
  bool ctrl = (n>=100);
  n=n%100;
  i=ui->RxFreqSpinBox->value();
  bool bTrackTx=ui->TxFreqSpinBox->value() == i;
  if(n==11) i--;
  if(n==12) i++;
  if (ui->RxFreqSpinBox->isEnabled ()) {
    ui->RxFreqSpinBox->setValue (i);
  }
  if(ctrl and m_mode=="WSPR") {
    ui->WSPRfreqSpinBox->setValue(i);
  } else {
    if(ctrl and bTrackTx) {
      ui->TxFreqSpinBox->setValue (i);
    }
  }
}

void MainWindow::displayDialFrequency ()
{
  Frequency dial_frequency {m_rigState.ptt () && m_rigState.split () ?
      m_rigState.tx_frequency () : m_rigState.frequency ()};

  // lookup band
  auto const& band_name = m_config.bands ()->find (dial_frequency);
  if (m_lastBand != band_name)
    {
      // only change this when necessary as we get called a lot and it
      // would trash any user input to the band combo box line edit
      ui->bandComboBox->setCurrentText (band_name.size () ? band_name : m_config.bands ()->oob ());
      m_wideGraph->setRxBand (band_name);
      m_lastBand = band_name;
      band_changed(dial_frequency);
    }

  // search working frequencies for one we are within 10kHz of (1 Mhz
  // of on VHF and up)
  bool valid {false};
  quint64 min_offset {99999999};
  for (auto const& item : *m_config.frequencies ())
    {
      // we need to do specific checks for above and below here to
      // ensure that we can use unsigned Radio::Frequency since we
      // potentially use the full 64-bit unsigned range.
      auto const& working_frequency = item.frequency_;
      auto const& offset = dial_frequency > working_frequency ?
        dial_frequency - working_frequency :
        working_frequency - dial_frequency;
      if (offset < min_offset) {
        min_offset = offset;
      }
    }
  if (min_offset < 10000u || (m_config.enable_VHF_features() && min_offset < 1000000u)) {
    valid = true;
  }

  update_dynamic_property (ui->labDialFreq, "oob", !valid);
  ui->labDialFreq->setText (Radio::pretty_frequency_MHz_string (dial_frequency));
}

void MainWindow::statusChanged()
{
  statusUpdate ();
  QFile f {m_config.temp_dir ().absoluteFilePath ("wsjtx_status.txt")};
  if(f.open(QFile::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    QString tmpGrid = m_hisGrid;
    if (!tmpGrid.size ()) tmpGrid="n/a"; // Not Available
    out << qSetRealNumberPrecision (12) << (m_freqNominal / 1.e6)
        << ";" << m_mode << ";" << m_hisCall << ";"
        << ui->rptSpinBox->value() << ";" << m_mode << ";" << tmpGrid
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        << Qt::endl
#else
        << endl
#endif
      ;
    f.close();
  } else {
    if (m_splash && m_splash->isVisible ()) m_splash->hide ();
    MessageBox::warning_message (this, tr ("Status File Error")
                                 , tr ("Cannot open \"%1\" for writing: %2")
                                 .arg (f.fileName ()).arg (f.errorString ()));
  }
  on_dxGridEntry_textChanged(m_hisGrid);
}

bool MainWindow::eventFilter (QObject * object, QEvent * event)
{
  switch (event->type())
    {
    case QEvent::KeyPress:
      // fall through
    case QEvent::MouseButtonPress:
      // reset the Tx watchdog
      tx_watchdog (false);
      break;

    case QEvent::ChildAdded:
      // ensure our child widgets get added to our event filter
      add_child_to_event_filter (static_cast<QChildEvent *> (event)->child ());
      break;

    case QEvent::ChildRemoved:
      // ensure our child widgets get d=removed from our event filter
      remove_child_from_event_filter (static_cast<QChildEvent *> (event)->child ());
      break;

    default: break;
    }
  return QObject::eventFilter(object, event);
}

void MainWindow::createStatusBar()                           //createStatusBar
{
  tx_status_label.setAlignment (Qt::AlignHCenter);
  tx_status_label.setMinimumSize (QSize  {100, 18});
  tx_status_label.setStyleSheet ("QLabel{color: #000000; background-color: #00ff00}");
  tx_status_label.setFrameStyle (QFrame::Panel | QFrame::Sunken);
  statusBar()->addWidget (&tx_status_label);

  config_label.setAlignment (Qt::AlignHCenter);
  config_label.setMinimumSize (QSize {80, 18});
  config_label.setFrameStyle (QFrame::Panel | QFrame::Sunken);
  statusBar()->addWidget (&config_label);
  config_label.hide ();         // only shown for non-default configuration

  mode_label.setAlignment (Qt::AlignHCenter);
  mode_label.setMinimumSize (QSize {80, 18});
  mode_label.setFrameStyle (QFrame::Panel | QFrame::Sunken);
  statusBar()->addWidget (&mode_label);

  last_tx_label.setAlignment (Qt::AlignHCenter);
  last_tx_label.setMinimumSize (QSize {150, 18});
  last_tx_label.setFrameStyle (QFrame::Panel | QFrame::Sunken);
  statusBar()->addWidget (&last_tx_label);

  ndecodes_label.setAlignment (Qt::AlignHCenter);
  ndecodes_label.setMinimumSize (QSize {30, 18});
  ndecodes_label.setFrameStyle (QFrame::Panel | QFrame::Sunken);
  statusBar()->addWidget (&ndecodes_label);

  band_hopping_label.setAlignment (Qt::AlignHCenter);
  band_hopping_label.setMinimumSize (QSize {90, 18});
  band_hopping_label.setFrameStyle (QFrame::Panel | QFrame::Sunken);

  statusBar()->addPermanentWidget(&progressBar);
  progressBar.setMinimumSize (QSize {150, 18});

  statusBar ()->addPermanentWidget (&watchdog_label);
  update_watchdog_label ();
}

void MainWindow::setup_status_bar (bool vhf)
{
  auto submode = current_submode ();
  if (vhf && submode != QChar::Null) {
    QString t{m_mode + " " + submode};
    if(m_mode=="Q65") t=m_mode + "-" + QString::number(m_TRperiod) + submode;
    mode_label.setText (t);
  } else {
    mode_label.setText (m_mode);
  } 
  if ("JT9" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #ff6ec7}");
  } else if ("JT4" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #cc99ff}");
  } else if ("Echo" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #66ffff}");
  } else if ("JT65" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #66ff66}");
  } else if ("Q65" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #99ff33}");
  } else if ("MSK144" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #ff6666}");
  } else if ("FT4" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #ff0099}");
  } else if ("FT8" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #ff6699}");
  } else if ("FST4" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #99ff66}");
  } else if ("FST4W" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #6699ff}");
  } else if ("FreqCal" == m_mode) {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #ff9933}");
  }
  last_tx_label.setText (QString {});
  if (m_mode.contains (QRegularExpression {R"(^(Echo))"})) {
    if (band_hopping_label.isVisible ()) statusBar ()->removeWidget (&band_hopping_label);
  } else if (m_mode=="WSPR") {
    mode_label.setStyleSheet ("QLabel{color: #000000; background-color: #ff66ff}");
    if (!band_hopping_label.isVisible ()) {
      statusBar ()->addWidget (&band_hopping_label);
      band_hopping_label.show ();
    }
  } else {
    if (band_hopping_label.isVisible ()) statusBar ()->removeWidget (&band_hopping_label);
  }
}

bool MainWindow::subProcessFailed (QProcess * process, int exit_code, QProcess::ExitStatus status)
{
  if (m_valid && (exit_code || QProcess::NormalExit != status))
    {
      QStringList arguments;
      for (auto argument: process->arguments ())
        {
          if (argument.contains (' ')) argument = '"' + argument + '"';
          arguments << argument;
        }
      if (m_splash && m_splash->isVisible ()) m_splash->hide ();
      MessageBox::critical_message (this, tr ("Subprocess Error")
                                    , tr ("Subprocess failed with exit code %1")
                                    .arg (exit_code)
                                    , tr ("Running: %1\n%2")
                                    .arg (process->program () + ' ' + arguments.join (' '))
                                    .arg (QString {process->readAllStandardError()}));
      return true;
    }
  return false;
}

void MainWindow::subProcessError (QProcess * process, QProcess::ProcessError)
{
  if (m_valid)
    {
      QStringList arguments;
      for (auto argument: process->arguments ())
        {
          if (argument.contains (' ')) argument = '"' + argument + '"';
          arguments << argument;
        }
      if (m_splash && m_splash->isVisible ()) m_splash->hide ();
      MessageBox::critical_message (this, tr ("Subprocess error")
                                    , tr ("Running: %1\n%2")
                                    .arg (process->program () + ' ' + arguments.join (' '))
                                    .arg (process->errorString ()));
      m_valid = false;              // ensures exit if still constructing
      QTimer::singleShot (0, this, SLOT (close ()));
    }
}

void MainWindow::closeEvent(QCloseEvent * e)
{
  m_valid = false;              // suppresses subprocess errors
  m_config.transceiver_offline ();
  writeSettings ();
  m_astroWidget.reset ();
  m_guiTimer.stop ();
  m_prefixes.reset ();
  m_shortcuts.reset ();
  m_mouseCmnds.reset ();
  m_colorHighlighting.reset ();
  if(m_mode!="MSK144" and m_mode!="FT8") killFile();
  float sw=0.0;
  int nw=400;
  int nh=100;
  int irow=-99;
  plotsave_(&sw,&nw,&nh,&irow);
  to_jt9(m_ihsym,999,-1);          //Tell jt9 to terminate
  if (!proc_jt9.waitForFinished(1000)) proc_jt9.close();
  mem_jt9->detach();
  Q_EMIT finished ();
  QMainWindow::closeEvent (e);
}

void MainWindow::on_stopButton_clicked()                       //stopButton
{
  monitor (false);
  m_loopall=false;
  if(m_bRefSpec) {
    MessageBox::information_message (this, tr ("Reference spectrum saved"));
    m_bRefSpec=false;
  }
}

void MainWindow::on_actionRelease_Notes_triggered ()
{
  QDesktopServices::openUrl (QUrl {"http://physics.princeton.edu/pulsar/k1jt/Release_Notes.txt"});
}

void MainWindow::on_actionFT8_DXpedition_Mode_User_Guide_triggered()
{
  QDesktopServices::openUrl (QUrl {"http://physics.princeton.edu/pulsar/k1jt/FT8_DXpedition_Mode.pdf"});
}

void MainWindow::on_actionQSG_FST4_triggered()
{
  QDesktopServices::openUrl (QUrl {"https://physics.princeton.edu/pulsar/k1jt/FST4_Quick_Start.pdf"});
}

void MainWindow::on_actionQSG_Q65_triggered()
{
  QDesktopServices::openUrl (QUrl {"https://physics.princeton.edu/pulsar/k1jt/Q65_Quick_Start.pdf"});
}

void MainWindow::on_actionQSG_X250_M3_triggered()
{
  QDesktopServices::openUrl (QUrl {"https://physics.princeton.edu/pulsar/k1jt/WSJTX_2.5.0_MAP65_3.0_Quick_Start.pdf"});
}

void MainWindow::on_actionOnline_User_Guide_triggered()      //Display manual
{
#if defined (CMAKE_BUILD)
  m_manual.display_html_url (QUrl {PROJECT_MANUAL_DIRECTORY_URL}, PROJECT_MANUAL);
#endif
}

//Display local copy of manual
void MainWindow::on_actionLocal_User_Guide_triggered()
{
#if defined (CMAKE_BUILD)
  m_manual.display_html_file (m_config.doc_dir (), PROJECT_MANUAL);
#endif
}

void MainWindow::on_actionWide_Waterfall_triggered()      //Display Waterfalls
{
  m_wideGraph->showNormal();
}

void MainWindow::on_actionEcho_Graph_triggered()
{
  m_echoGraph->showNormal();
}

void MainWindow::on_actionFast_Graph_triggered()
{
  m_fastGraph->showNormal();
}

void MainWindow::on_actionSolve_FreqCal_triggered()
{
  auto data_dir {QDir::toNativeSeparators(m_config.writeable_data_dir().absolutePath()).toLocal8Bit ()};
  int iz,irc;
  double a,b,rms,sigmaa,sigmab;
  calibrate_(data_dir.constData (),&iz,&a,&b,&rms,&sigmaa,&sigmab,&irc,data_dir.size ());
  QString t2;
  if(irc==-1) t2="Cannot open " + data_dir + "/fmt.all";
  if(irc==-2) t2="Cannot open " + data_dir + "/fcal2.out";
  if(irc==-3) t2="Insufficient data in fmt.all";
  if(irc==-4) t2 = tr ("Invalid data in fmt.all at line %1").arg (iz);
  if(irc>0 or rms>1.0) t2="Check fmt.all for possible bad data.";
  if (irc < 0 || irc > 0 || rms > 1.) {
    MessageBox::warning_message (this, "Calibration Error", t2);
  }
  else if (MessageBox::Apply == MessageBox::query_message (this
                                                           , tr ("Good Calibration Solution")
                                                           , tr ("<pre>"
                                                                 "%1%L2 ±%L3 ppm\n"
                                                                 "%4%L5 ±%L6 Hz\n\n"
                                                                 "%7%L8\n"
                                                                 "%9%L10 Hz"
                                                                 "</pre>")
                                                           .arg ("Slope: ", 12).arg (b, 0, 'f', 3).arg (sigmab, 0, 'f', 3)
                                                           .arg ("Intercept: ", 12).arg (a, 0, 'f', 2).arg (sigmaa, 0, 'f', 2)
                                                           .arg ("N: ", 12).arg (iz)
                                                           .arg ("StdDev: ", 12).arg (rms, 0, 'f', 2)
                                                           , QString {}
                                                           , MessageBox::Cancel | MessageBox::Apply)) {
    m_config.set_calibration (Configuration::CalibrationParams {a, b});
    if (MessageBox::Yes == MessageBox::query_message (this
                                                      , tr ("Delete Calibration Measurements")
                                                      , tr ("The \"fmt.all\" file will be renamed as \"fmt.bak\""))) {
      // rename fmt.all as we have consumed the resulting calibration
      // solution
      auto const& backup_file_name = m_config.writeable_data_dir ().absoluteFilePath ("fmt.bak");
      QFile::remove (backup_file_name);
      QFile::rename (m_config.writeable_data_dir ().absoluteFilePath ("fmt.all"), backup_file_name);
    }
  }
}

void MainWindow::on_actionCopyright_Notice_triggered()
{
  auto const& message = tr("If you make fair use of any part of WSJT-X under terms of the GNU "
                           "General Public License, you must display the following copyright "
                           "notice prominently in your derivative work:\n\n"
                           "\"The algorithms, source code, look-and-feel of WSJT-X and related "
                           "programs, and protocol specifications for the modes FSK441, FST4, FT8, "
                           "JT4, JT6M, JT9, JT65, JTMS, QRA64, Q65, MSK144 are Copyright (C) "
                           "2001-2021 by one or more of the following authors: Joseph Taylor, "
                           "K1JT; Bill Somerville, G4WJS; Steven Franke, K9AN; Nico Palermo, "
                           "IV3NWV; Greg Beam, KI7MT; Michael Black, W9MDB; Edson Pereira, PY2SDR; "
                           "Philip Karn, KA9Q; and other members of the WSJT Development Group.\"");
  MessageBox::warning_message(this, message);
}

// Implement the MultiGeometryWidget::change_layout() operation.
void MainWindow::change_layout (std::size_t n)
{
  switch (n)
    {
    case 1:                     // SWL view
      ui->menuBar->show ();
      ui->lower_panel_widget->hide ();
      trim_view (false);        // ensure we can switch back
      break;

    case 2:                     // hide menus view
      ui->menuBar->hide ();
      ui->lower_panel_widget->show ();
      trim_view (true);
      break;

    default:                    // normal view
      ui->menuBar->setVisible (ui->cbMenus->isChecked ());
      ui->lower_panel_widget->show ();
      trim_view (!ui->cbMenus->isChecked ());
      break;
    }
}

void MainWindow::on_actionSWL_Mode_triggered (bool checked)
{
  select_geometry (checked ? 1 : ui->cbMenus->isChecked () ? 0 : 2);
}

// This allows the window to shrink by removing certain things
// and reducing space used by controls
void MainWindow::trim_view (bool checked)
{
  int spacing = checked ? 1 : 6;
  if (checked) {
      statusBar ()->removeWidget (&auto_tx_label);
  } else {
      statusBar ()->addWidget(&auto_tx_label);
  }
  if (m_mode != "FreqCal" && m_mode != "WSPR" && m_mode != "FST4W") {
    ui->lh_decodes_title_label->setVisible(!checked);
    ui->rh_decodes_title_label->setVisible(!checked);
  }
  ui->lh_decodes_headings_label->setVisible(!checked);
  ui->rh_decodes_headings_label->setVisible(!checked);
  ui->gridLayout_5->layout()->setSpacing(spacing);
  ui->horizontalLayout_2->layout()->setSpacing(spacing);
  ui->horizontalLayout_5->layout()->setSpacing(spacing);
  ui->horizontalLayout_6->layout()->setSpacing(spacing);
  ui->horizontalLayout_7->layout()->setSpacing(spacing);
  ui->horizontalLayout_8->layout()->setSpacing(spacing);
  ui->horizontalLayout_9->layout()->setSpacing(spacing);
  ui->horizontalLayout_10->layout()->setSpacing(spacing);
  ui->horizontalLayout_11->layout()->setSpacing(spacing);
  ui->horizontalLayout_12->layout()->setSpacing(spacing);
  ui->horizontalLayout_13->layout()->setSpacing(spacing);
  ui->horizontalLayout_14->layout()->setSpacing(spacing);
  ui->rh_decodes_widget->layout()->setSpacing(spacing);
  ui->verticalLayout_2->layout()->setSpacing(spacing);
  ui->verticalLayout_3->layout()->setSpacing(spacing);
  ui->verticalLayout_5->layout()->setSpacing(spacing);
  ui->verticalLayout_7->layout()->setSpacing(spacing);
  ui->verticalLayout_8->layout()->setSpacing(spacing);
  ui->tab->layout()->setSpacing(spacing);
}

void MainWindow::on_actionAstronomical_data_toggled (bool checked)
{
  if (checked)
    {
      m_astroWidget.reset (new Astro {m_settings, &m_config});

      // hook up termination signal
      connect (this, &MainWindow::finished, m_astroWidget.data (), &Astro::close);
      connect (m_astroWidget.data (), &Astro::tracking_update, [this] {
          m_astroCorrection = {};
          setRig ();
          setXIT (ui->TxFreqSpinBox->value ());
          displayDialFrequency ();
        });
      m_astroWidget->showNormal();
      m_astroWidget->raise ();
      m_astroWidget->activateWindow ();
      m_astroWidget->nominal_frequency (m_freqNominal, m_freqTxNominal);
    }
  else
    {
      m_astroWidget.reset ();
    }
}

void MainWindow::on_fox_log_action_triggered()
{
  if (!m_foxLogWindow)
    {
      m_foxLogWindow.reset (new FoxLogWindow {m_settings, &m_config, m_logBook.fox_log ()});

      // Connect signals from fox log window
      connect (this, &MainWindow::finished, m_foxLogWindow.data (), &FoxLogWindow::close);
      connect (m_foxLogWindow.data (), &FoxLogWindow::reset_log_model, [this] () {
          m_logBook.fox_log ()->reset ();
        });
    }
  m_foxLogWindow->showNormal ();
  m_foxLogWindow->raise ();
  m_foxLogWindow->activateWindow ();
}

void MainWindow::on_contest_log_action_triggered()
{
  if (!m_contestLogWindow)
    {
      m_contestLogWindow.reset (new CabrilloLogWindow {m_settings, &m_config, m_logBook.contest_log ()->model ()});

      // Connect signals from contest log window
      connect (this, &MainWindow::finished, m_contestLogWindow.data (), &CabrilloLogWindow::close);
    }
  m_contestLogWindow->showNormal ();
  m_contestLogWindow->raise ();
  m_contestLogWindow->activateWindow ();
}

void MainWindow::on_actionColors_triggered()
{
  if (!m_colorHighlighting)
    {
      m_colorHighlighting.reset (new ColorHighlighting {m_settings, m_config.decode_highlighting ()});
      connect (&m_config, &Configuration::decode_highlighting_changed, m_colorHighlighting.data (), &ColorHighlighting::set_items);
    }
  m_colorHighlighting->showNormal ();
  m_colorHighlighting->raise ();
  m_colorHighlighting->activateWindow ();
}

void MainWindow::on_actionMessage_averaging_triggered()
{
  if(m_msgAvgWidget == NULL) {
    m_msgAvgWidget.reset (new MessageAveraging {m_settings, m_config.decoded_text_font ()});

    // Connect signals from Message Averaging window
    connect (this, &MainWindow::finished, m_msgAvgWidget.data (), &MessageAveraging::close);
  }
  m_msgAvgWidget->showNormal();
  m_msgAvgWidget->raise();
  m_msgAvgWidget->activateWindow();
}

void MainWindow::on_actionOpen_triggered()                     //Open File
{
  monitor (false);

  QString fname;
  fname=QFileDialog::getOpenFileName(this, "Open File", m_path,
                                     "WSJT Files (*.wav)");
  if(!fname.isEmpty ()) {
    m_path=fname;
    int i1=fname.lastIndexOf("/");
    QString baseName=fname.mid(i1+1);
    tx_status_label.setStyleSheet("QLabel{color: #000000; background-color: #99ffff}");
    tx_status_label.setText(" " + baseName + " ");
    on_stopButton_clicked();
    m_diskData=true;
    read_wav_file (fname);
  }
}

void MainWindow::read_wav_file (QString const& fname)
{
  // call diskDat() when done
  int i0=fname.lastIndexOf("_");
  int i1=fname.indexOf(".wav");
  m_nutc0=m_UTCdisk;
  m_UTCdisk=fname.mid(i0+1,i1-i0-1).toInt();
  m_wav_future_watcher.setFuture (QtConcurrent::run ([this, fname] {
        auto basename = fname.mid (fname.lastIndexOf ('/') + 1);
        auto pos = fname.indexOf (".wav", 0, Qt::CaseInsensitive);
        // global variables and threads do not mix well, this needs changing
        dec_data.params.nutc = 0;
        if (pos > 0) {
          if (pos == fname.indexOf ('_', -11) + 7) {
            dec_data.params.nutc = fname.mid (pos - 6, 6).toInt ();
            m_fileDateTime=fname.mid(pos-13,13);
          } else {
            dec_data.params.nutc = 100 * fname.mid (pos - 4, 4).toInt ();
            m_fileDateTime=fname.mid(pos-11,11);
          }
        }

        BWFFile file {QAudioFormat {}, fname};
        bool ok=file.open (BWFFile::ReadOnly);
        if(ok) {
          auto bytes_per_frame = file.format ().bytesPerFrame ();
          int nsamples=m_TRperiod * RX_SAMPLE_RATE;
          qint64 max_bytes = std::min (std::size_t (nsamples),
              sizeof (dec_data.d2) / sizeof (dec_data.d2[0]))* bytes_per_frame;
          auto n = file.read (reinterpret_cast<char *> (dec_data.d2),
                            std::min (max_bytes, file.size ()));
          int frames_read = n / bytes_per_frame;
        // zero unfilled remaining sample space
          std::memset(&dec_data.d2[frames_read],0,max_bytes - n);
          if (11025 == file.format ().sampleRate ()) {
            short sample_size = file.format ().sampleSize ();
            wav12_ (dec_data.d2, dec_data.d2, &frames_read, &sample_size);
          }
          dec_data.params.kin = frames_read;
          dec_data.params.newdat = 1;
        } else {
          dec_data.params.kin = 0;
          dec_data.params.newdat = 0;
        }

        if(basename.mid(0,10)=="000000_000" && m_mode == "FT8") {
          int isec=15*basename.mid(10,3).toInt();
          int ih=isec/3600;
          int im=(isec-3600*ih)/60;
          isec=isec%60;
          dec_data.params.nutc=3600*ih+60*im+isec;
        }

      }));
}

void MainWindow::on_actionOpen_next_in_directory_triggered()   //Open Next
{
  if(m_decoderBusy) return;
  monitor (false);
  int i,len;
  QFileInfo fi(m_path);
  QStringList list;
  list= fi.dir().entryList().filter(".wav",Qt::CaseInsensitive);
  for (i = 0; i < list.size()-1; ++i) {
    len=list.at(i).length();
    if(list.at(i)==m_path.right(len)) {
      int n=m_path.length();
      QString fname=m_path.replace(n-len,len,list.at(i+1));
      m_path=fname;
      int i1=fname.lastIndexOf("/");
      QString baseName=fname.mid(i1+1);
      tx_status_label.setStyleSheet("QLabel{color: #000000; background-color: #99ffff}");
      tx_status_label.setText(" " + baseName + " ");
      m_diskData=true;
      read_wav_file (fname);
      if(m_loopall and (i==list.size()-2)) {
        m_loopall=false;
        m_bNoMoreFiles=true;
      }
      return;
    }
  }
}
//Open all remaining files
void MainWindow::on_actionDecode_remaining_files_in_directory_triggered()
{
  if(m_decoderBusy) return;
  m_loopall=true;
  on_actionOpen_next_in_directory_triggered();
}

void MainWindow::diskDat()                                   //diskDat()
{
  m_wideGraph->setDiskUTC(dec_data.params.nutc);
  if(dec_data.params.kin>0) {
    int k;
    int kstep=m_FFTSize;
    m_diskData=true;
    float db=m_config.degrade();
    float bw=m_config.RxBandwidth();
    if(db > 0.0) degrade_snr_(dec_data.d2,&dec_data.params.kin,&db,&bw);
    for(int n=1; n<=m_hsymStop; n++) {                      // Do the waterfall spectra
//      k=(n+1)*kstep;           //### Why was this (n+1) ??? ###
      k=n*kstep;
      if(k > dec_data.params.kin) break;
      dec_data.params.npts8=k/8;
      dataSink(k);
      qApp->processEvents();                                //Update the waterfall
    }
  } else {
    MessageBox::information_message(this, tr("No data read from disk. Wrong file format?"));
  }
}

//Delete ../save/*.wav
void MainWindow::on_actionDelete_all_wav_files_in_SaveDir_triggered()
{
  auto button = MessageBox::query_message (this, tr ("Confirm Delete"),
                                             tr ("Are you sure you want to delete all *.wav and *.c2 files in \"%1\"?")
                                             .arg (QDir::toNativeSeparators (m_config.save_directory ().absolutePath ())));
  if (MessageBox::Yes == button) {
    Q_FOREACH (auto const& file
               , m_config.save_directory ().entryList ({"*.wav", "*.c2"}, QDir::Files | QDir::Writable)) {
      m_config.save_directory ().remove (file);
    }
  }
}

void MainWindow::on_actionNone_triggered()                    //Save None
{
  m_saveDecoded=false;
  m_saveAll=false;
  ui->actionNone->setChecked(true);
}

void MainWindow::on_actionSave_decoded_triggered()
{
  m_saveDecoded=true;
  m_saveAll=false;
  ui->actionSave_decoded->setChecked(true);
}

void MainWindow::on_actionSave_all_triggered()                //Save All
{
  m_saveDecoded=false;
  m_saveAll=true;
  ui->actionSave_all->setChecked(true);
}

void MainWindow::on_actionKeyboard_shortcuts_triggered()
{
  if (!m_shortcuts)
    {
      QFont font;
      font.setPointSize (10);
      m_shortcuts.reset (new HelpTextWindow {tr ("Keyboard Shortcuts"),
                                               //: Keyboard shortcuts help window contents
                                               tr (R"(<table cellspacing=1>
  <tr><td><b>Esc      </b></td><td>Stop Tx, abort QSO, clear next-call queue</td></tr>
  <tr><td><b>F1       </b></td><td>Online User's Guide (Alt: transmit Tx6)</td></tr>
  <tr><td><b>Shift+F1  </b></td><td>Copyright Notice</td></tr>
  <tr><td><b>Ctrl+F1  </b></td><td>About WSJT-X</td></tr>
  <tr><td><b>F2       </b></td><td>Open settings window (Alt: transmit Tx2)</td></tr>
  <tr><td><b>F3       </b></td><td>Display keyboard shortcuts (Alt: transmit Tx3)</td></tr>
  <tr><td><b>F4       </b></td><td>Clear DX Call, DX Grid, Tx messages 1-4 (Alt: transmit Tx4)</td></tr>
  <tr><td><b>Alt+F4   </b></td><td>Exit program</td></tr>
  <tr><td><b>F5       </b></td><td>Display special mouse commands (Alt: transmit Tx5)</td></tr>
  <tr><td><b>F6       </b></td><td>Open next file in directory (Alt: toggle "Call 1st")</td></tr>
  <tr><td><b>Shift+F6 </b></td><td>Decode all remaining files in directory</td></tr>
  <tr><td><b>F7       </b></td><td>Display Message Averaging window</td></tr>
  <tr><td><b>F11      </b></td><td>Move Rx frequency down 1 Hz</td></tr>
  <tr><td><b>Ctrl+F11 </b></td><td>Move identical Rx and Tx frequencies down 1 Hz</td></tr>
  <tr><td><b>Shift+F11 </b></td><td>Move Tx frequency down 60 Hz (FT8) or 90 Hz (FT4)</td></tr>
  <tr><td><b>Ctrl+Shift+F11 </b></td><td>Move dial frequency down 2000 Hz</td></tr>
  <tr><td><b>F12      </b></td><td>Move Rx frequency up 1 Hz</td></tr>
  <tr><td><b>Ctrl+F12 </b></td><td>Move identical Rx and Tx frequencies up 1 Hz</td></tr>
  <tr><td><b>Shift+F12 </b></td><td>Move Tx frequency up 60 Hz (FT8) or 90 Hz (FT4)</td></tr>
  <tr><td><b>Ctrl+Shift+F12 </b></td><td>Move dial frequency up 2000 Hz</td></tr>
  <tr><td><b>Alt+1-6  </b></td><td>Set now transmission to this number on Tab 1</td></tr>
  <tr><td><b>Ctl+1-6  </b></td><td>Set next transmission to this number on Tab 1</td></tr>
  <tr><td><b>Alt+B    </b></td><td>Toggle "Best S+P" status</td></tr>
  <tr><td><b>Alt+C    </b></td><td>Toggle "Call 1st" checkbox</td></tr>
  <tr><td><b>Alt+D    </b></td><td>Decode again at QSO frequency</td></tr>
  <tr><td><b>Shift+D  </b></td><td>Full decode (both windows)</td></tr>
  <tr><td><b>Ctrl+E   </b></td><td>Turn on TX even/1st</td></tr>
  <tr><td><b>Shift+E  </b></td><td>Turn off TX even/1st</td></tr>
  <tr><td><b>Alt+E    </b></td><td>Erase</td></tr>
  <tr><td><b>Ctrl+F   </b></td><td>Edit the free text message box</td></tr>
  <tr><td><b>Alt+G    </b></td><td>Generate standard messages</td></tr>
  <tr><td><b>Alt+H    </b></td><td>Halt Tx</td></tr>
  <tr><td><b>Ctrl+L   </b></td><td>Lookup callsign in database, generate standard messages</td></tr>
  <tr><td><b>Alt+M    </b></td><td>Monitor</td></tr>
  <tr><td><b>Alt+N    </b></td><td>Enable Tx</td></tr>
  <tr><td><b>Ctrl+O   </b></td><td>Open a .wav file</td></tr>
  <tr><td><b>Alt+O    </b></td><td>Change operator</td></tr>
  <tr><td><b>Alt+Q    </b></td><td>Log QSO</td></tr>
  <tr><td><b>Ctrl+R   </b></td><td>Set Tx4 message to RRR (not in FT4)</td></tr>
  <tr><td><b>Alt+R    </b></td><td>Set Tx4 message to RR73</td></tr>
  <tr><td><b>Alt+S    </b></td><td>Stop monitoring</td></tr>
  <tr><td><b>Alt+T    </b></td><td>Toggle Tune status</td></tr>
  <tr><td><b>Alt+Z    </b></td><td>Clear hung decoder status</td></tr>
</table>)"), font});
    }
  m_shortcuts->showNormal ();
  m_shortcuts->raise ();
}

void MainWindow::on_actionSpecial_mouse_commands_triggered()
{
  if (!m_mouseCmnds)
    {
      QFont font;
      font.setPointSize (10);
      m_mouseCmnds.reset (new HelpTextWindow {tr ("Special Mouse Commands"),
                                                //: Mouse commands help window contents
                                                tr (R"(<table cellpadding=5>
  <tr>
    <th align="right">Click on</th>
    <th align="left">Action</th>
  </tr>
  <tr>
    <td align="right">Waterfall:</td>
    <td><b>Click</b> to set Rx frequency.<br/>
        <b>Shift-click</b> to set Tx frequency.<br/>
        <b>Ctrl-click</b> or <b>Right-click</b> to set Rx and Tx frequencies.<br/>
        <b>Double-click</b> to also decode at Rx frequency.<br/>
    </td>
  </tr>
  <tr>
    <td align="right">Decoded text:</td>
    <td><b>Double-click</b> to copy second callsign to Dx Call,<br/>
        locator to Dx Grid, change Rx and Tx frequency to<br/>
        decoded signal's frequency, and generate standard<br/>
        messages.<br/>
        If <b>Hold Tx Freq</b> is checked or first callsign in message<br/>
        is your own call, Tx frequency is not changed unless <br/>
        <b>Ctrl</b> is held down.<br/>
    </td>
  </tr>
  <tr>
    <td align="right">Erase button:</td>
    <td><b>Click</b> to erase QSO window.<br/>
        <b>Double-click</b> to erase QSO and Band Activity windows.
    </td>
  </tr>
</table>)"), font});
    }
  m_mouseCmnds->showNormal ();
  m_mouseCmnds->raise ();
}

void MainWindow::on_DecodeButton_clicked (bool /* checked */) //Decode request
{
  if(m_mode=="MSK144") {
    ui->DecodeButton->setChecked(false);
  } else {
    if(m_mode!="WSPR" && !m_decoderBusy) {
      dec_data.params.newdat=0;
      dec_data.params.nagain=1;
      decode();
    }
  }
}

void MainWindow::freezeDecode(int n)                          //freezeDecode()
{
  if((n%100)==2) {
    if(m_mode=="FST4" and m_config.single_decode() and ui->sbFtol->value()>10) ui->sbFtol->setValue(10);
    on_DecodeButton_clicked (true);
  }
}

void MainWindow::on_ClrAvgButton_clicked()
{
  m_nclearave=1;
  if(m_msgAvgWidget != NULL) {
    if(m_msgAvgWidget->isVisible()) m_msgAvgWidget->displayAvg("");
  }
  if(m_mode=="Q65") ndecodes_label.setText("0  0");
}

void MainWindow::msgAvgDecode2()
{
  on_DecodeButton_clicked (true);
}

void MainWindow::decode()                                       //decode()
{
  if(m_decoderBusy) return;                          //Don't start decoder if it's already busy.
  QDateTime now = QDateTime::currentDateTimeUtc ();
  if( m_dateTimeLastTX.isValid () ) {
    qint64 isecs_since_tx = m_dateTimeLastTX.secsTo(now);
    dec_data.params.lapcqonly= (isecs_since_tx > 300); 
  } else { 
    m_dateTimeLastTX = now.addSecs(-900);
    dec_data.params.lapcqonly=true;
  }
  if( m_diskData ) {
    dec_data.params.lapcqonly=false;
  }
  if(!m_dataAvailable or m_TRperiod==0.0) return;
  ui->DecodeButton->setChecked (true);
  if(!dec_data.params.nagain && m_diskData && m_TRperiod >= 60.) {
    dec_data.params.nutc=dec_data.params.nutc/100;
  }
  if(dec_data.params.nagain==0 && dec_data.params.newdat==1 && (!m_diskData)) {
    m_dateTimeSeqStart = qt_truncate_date_time_to (QDateTime::currentDateTimeUtc (), m_TRperiod * 1.e3);
    auto t = m_dateTimeSeqStart.time ();
    dec_data.params.nutc = t.hour () * 100 + t.minute ();
    if (m_TRperiod < 60.)
      {
        dec_data.params.nutc = dec_data.params.nutc * 100 + t.second ();
      }
  }

  if(m_nPick==1 and !m_diskData) {
    QDateTime t=QDateTime::currentDateTimeUtc();
    int ihr=t.toString("hh").toInt();
    int imin=t.toString("mm").toInt();
    int isec=t.toString("ss").toInt();
    isec=isec - fmod(double(isec),m_TRperiod);
    dec_data.params.nutc=10000*ihr + 100*imin + isec;
  }
  if(m_nPick==2) dec_data.params.nutc=m_nutc0;
  dec_data.params.nQSOProgress = m_QSOProgress;
  dec_data.params.nfqso=m_wideGraph->rxFreq();
  dec_data.params.nftx = ui->TxFreqSpinBox->value ();
  qint32 depth {m_ndepth};
  if (!ui->actionInclude_averaging->isVisible ()) depth &= ~16;
  if (!ui->actionInclude_correlation->isVisible ()) depth &= ~32;
  if (!ui->actionEnable_AP_DXcall->isVisible ()) depth &= ~64;
  if (!ui->actionAuto_Clear_Avg->isVisible()) depth &= ~128;
  dec_data.params.ndepth=depth;
  dec_data.params.n2pass=1;
  if(m_config.twoPass()) dec_data.params.n2pass=2;
  dec_data.params.nranera=m_config.ntrials();
  dec_data.params.naggressive=m_config.aggressive();
  dec_data.params.nrobust=0;
  dec_data.params.ndiskdat=0;
  if(m_diskData) dec_data.params.ndiskdat=1;
  dec_data.params.nfa=m_wideGraph->nStartFreq();
  dec_data.params.nfSplit=m_wideGraph->Fmin();
  dec_data.params.nfb=m_wideGraph->Fmax();
  if(m_mode=="FT8" and SpecOp::HOUND == m_config.special_op_id() and !ui->cbRxAll->isChecked()) dec_data.params.nfb=1000;
  if(m_mode=="FT8" and SpecOp::FOX == m_config.special_op_id() ) dec_data.params.nfqso=200;
  dec_data.params.ntol=ui->sbFtol->value ();
  if(!m_config.enable_VHF_features()) {
    dec_data.params.ntol=20;
    dec_data.params.naggressive=0;
  }
  if(m_mode=="FST4") {
    dec_data.params.ntol=ui->sbFtol->value();
    if(m_config.single_decode()) {
      dec_data.params.nfa=m_wideGraph->rxFreq() - ui->sbFtol->value();
      dec_data.params.nfb=m_wideGraph->rxFreq() + ui->sbFtol->value();
    } else {
      dec_data.params.nfa=ui->sbF_Low->value();
      dec_data.params.nfb=ui->sbF_High->value();
    }
  }
  if(m_mode=="FST4W") dec_data.params.ntol=ui->sbFST4W_FTol->value();
  if(dec_data.params.nutc < m_nutc0) m_RxLog = 1;       //Date and Time to file "ALL.TXT".
  if(dec_data.params.newdat==1 and !m_diskData) m_nutc0=dec_data.params.nutc;
  dec_data.params.ntxmode=9;
  dec_data.params.nmode=9;
  if(m_mode=="JT65") dec_data.params.nmode=65;
  if(m_mode=="JT65") dec_data.params.ljt65apon = ui->actionEnable_AP_JT65->isVisible () &&
      ui->actionEnable_AP_JT65->isChecked ();
  if(m_mode=="Q65") dec_data.params.nmode=66;
  if(m_mode=="Q65") dec_data.params.ntxmode=66;
  if(m_mode=="JT4") {
    dec_data.params.nmode=4;
    dec_data.params.ntxmode=4;
  }
  if(m_mode=="FT8") dec_data.params.nmode=8;
  if(m_mode=="FT8") dec_data.params.lft8apon = ui->actionEnable_AP_FT8->isVisible () &&
      ui->actionEnable_AP_FT8->isChecked ();
  if(m_mode=="FT8") dec_data.params.napwid=50;
  if(m_mode=="FT4") {
    dec_data.params.nmode=5;
    m_BestCQpriority="";
  }
  if(m_mode=="FST4") dec_data.params.nmode=240;
  if(m_mode=="FST4W") dec_data.params.nmode=241;
  dec_data.params.ntxmode=dec_data.params.nmode;   // Is this used any more?
  dec_data.params.ntrperiod=m_TRperiod;
  dec_data.params.nsubmode=m_nSubMode;
  dec_data.params.minw=0;
  dec_data.params.nclearave=m_nclearave;
  if(m_nclearave!=0) {
    QFile f(m_config.temp_dir ().absoluteFilePath ("avemsg.txt"));
    f.remove();
  }
  dec_data.params.dttol=m_DTtol;
  dec_data.params.emedelay=0.0;
  if(m_config.decode_at_52s()) dec_data.params.emedelay=2.5;
  dec_data.params.minSync=ui->syncSpinBox->isVisible () ? m_minSync : 0;
  dec_data.params.nexp_decode = static_cast<int> (m_config.special_op_id());
  if(m_config.single_decode()) dec_data.params.nexp_decode += 32;
  if(m_config.enable_VHF_features()) dec_data.params.nexp_decode += 64;
  if(m_mode.startsWith("FST4")) dec_data.params.nexp_decode += 256*(ui->sbNB->value()+3);
  dec_data.params.max_drift=ui->sbMaxDrift->value();

  ::memcpy(dec_data.params.datetime, m_dateTime.toLatin1()+"    ", sizeof dec_data.params.datetime);
  ::memcpy(dec_data.params.mycall, (m_config.my_callsign()+"            ").toLatin1(), sizeof dec_data.params.mycall);
  ::memcpy(dec_data.params.mygrid, (m_config.my_grid()+"      ").toLatin1(), sizeof dec_data.params.mygrid);
  QString hisCall {ui->dxCallEntry->text ()};
  QString hisGrid {ui->dxGridEntry->text ()};
  memcpy(dec_data.params.hiscall,(hisCall + "            ").toLatin1 ().constData (), sizeof dec_data.params.hiscall);
  memcpy(dec_data.params.hisgrid,(hisGrid + "      ").toLatin1 ().constData (), sizeof dec_data.params.hisgrid);

  //newdat=1  ==> this is new data, must do the big FFT
  //nagain=1  ==> decode only at fQSO +/- Tol

  if (auto * to = reinterpret_cast<char *> (mem_jt9->data()))
    {
      char *from = (char*) dec_data.ipc;
      int size=sizeof(struct dec_data);
      if(dec_data.params.newdat==0) {
        int noffset {offsetof (struct dec_data, params.nutc)};
        to += noffset;
        from += noffset;
        size -= noffset;
      }
      if(m_mode=="MSK144" or m_bFast9) {
        float t0=m_t0;
        float t1=m_t1;
        qApp->processEvents();                                //Update the waterfall
        if(m_nPick > 0) {
          t0=m_t0Pick;
          t1=m_t1Pick;
        }
        static short int d2b[360000];
        narg[0]=dec_data.params.nutc;
        if(m_kdone>int(12000.0*m_TRperiod)) {
          m_kdone=int(12000.0*m_TRperiod);
        }
        narg[1]=m_kdone;
        narg[2]=m_nSubMode;
        narg[3]=dec_data.params.newdat;
        narg[4]=dec_data.params.minSync;
        narg[5]=m_nPick;
        narg[6]=1000.0*t0;
        narg[7]=1000.0*t1;
        narg[8]=2;                                //Max decode lines per decode attempt
        if(dec_data.params.minSync<0) narg[8]=50;
        if(m_mode=="JT9") narg[9]=102;            //Fast JT9
        if(m_mode=="MSK144") narg[9]=104;         //MSK144
        narg[10]=ui->RxFreqSpinBox->value();
        narg[11]=ui->sbFtol->value ();
        narg[12]=0;
        narg[13]=-1;
        narg[14]=m_config.aggressive();
        memcpy(d2b,dec_data.d2,2*360000);
        watcher3.setFuture (QtConcurrent::run (std::bind (fast_decode_,&d2b[0],
                                                          &narg[0],&m_TRperiod,&m_msg[0][0],
                                                          dec_data.params.mycall,dec_data.params.hiscall,8000,12,12)));
      } else {
        mem_jt9->lock ();
        memcpy(to, from, qMin(mem_jt9->size(), size));
        mem_jt9->unlock ();
        to_jt9(m_ihsym,1,-1);                //Send m_ihsym to jt9[.exe] and start decoding
        decodeBusy(true);
      }
    }
}

void::MainWindow::fast_decode_done()
{
  float t,tmax=-99.0;
  dec_data.params.nagain=false;
  dec_data.params.ndiskdat=false;
//  if(m_msg[0][0]==0) m_bDecoded=false;
  for(int i=0; m_msg[i][0] && i<100; i++) {
    QString message=QString::fromLatin1(m_msg[i]);
    m_msg[i][0]=0;
    if(message.length()>80) message=message.left (80);
    if(narg[13]/8==narg[12]) message=message.trimmed().replace("<...>",m_calls);

//Left (Band activity) window
    DecodedText decodedtext {message.replace (QChar::LineFeed, "")};
    if(!m_bFastDone) {
      ui->decodedTextBrowser->displayDecodedText (decodedtext, m_config.my_callsign (), m_mode, m_config.DXCC (),
         m_logBook, m_currentBand, m_config.ppfx ());
    }

    t=message.mid(10,5).toFloat();
    if(t>tmax) {
      tmax=t;
      m_bDecoded=true;
    }
    postDecode (true, decodedtext.string ());
    write_all("Rx",message);

    if(m_mode=="JT9" or m_mode=="MSK144") {
// find and extract any report for myCall
      bool stdMsg = decodedtext.report(m_baseCall,
                    Radio::base_callsign(ui->dxCallEntry->text()), m_rptRcvd);

// extract details and send to PSKreporter
      if (stdMsg) pskPost (decodedtext);
    }
    if (tmax >= 0.0) auto_sequence (decodedtext, ui->sbFtol->value (), ui->sbFtol->value ());
  }
  m_startAnother=m_loopall;
  m_nPick=0;
  ui->DecodeButton->setChecked (false);
  m_bFastDone=false;
}

void MainWindow::to_jt9(qint32 n, qint32 istart, qint32 idone)
{
  if (auto * dd = reinterpret_cast<dec_data_t *> (mem_jt9->data()))
    {
      mem_jt9->lock ();
      dd->ipc[0]=n;
      if(istart>=0) dd->ipc[1]=istart;
      if(idone>=0)  dd->ipc[2]=idone;
      mem_jt9->unlock ();
    }
}

void MainWindow::decodeDone ()
{
  if(m_mode=="Q65") m_wideGraph->drawRed(0,0);
  if ("FST4W" == m_mode)
    {
      if (m_uploadWSPRSpots
          && m_config.is_transceiver_online ()) { // need working rig control
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        uploadTimer.start(QRandomGenerator::global ()->bounded (0, 20000)); // Upload delay
#else
        uploadTimer.start(20000 * qrand()/((double)RAND_MAX + 1.0)); // Upload delay
#endif
      }
    }
  auto tnow = QDateTime::currentDateTimeUtc ();
  double tdone = fmod(double(tnow.time().second()),m_TRperiod);
  int mswait;
  if( tdone < 0.5*m_TRperiod ) {
    mswait = 1000.0 * ( 0.6 * m_TRperiod - tdone );
  } else {
    mswait = 1000.0 * ( 1.6 * m_TRperiod - tdone );
  }
  m_bDecoded=m_nDecodes>0;
//  qDebug() << "aa 3316" << m_saveDecoded << m_saveAll << m_bDecoded << m_nDecodes
//           << m_TRperiod << tdone << mswait;
  if(!m_diskData and !m_saveAll) {
    if(m_saveDecoded and (m_nDecodes==0)) {
//      qDebug() << "bb 3319" << mswait;
      killFileTimer.start(mswait); //Kill at 3/4 period
    }
  }
  if(m_mode!="FT8" or dec_data.params.nzhsym==50) m_nDecodes=0;

  dec_data.params.nagain=0;
  dec_data.params.ndiskdat=0;
  m_nclearave=0;
//  pause_jt9 ();
  ui->DecodeButton->setChecked (false);
  decodeBusy(false);
  m_RxLog=0;
  if(SpecOp::FOX == m_config.special_op_id()) houndCallers();
  to_jt9(m_ihsym,-1,1);                //Tell jt9 we know it has finished

  m_startAnother=m_loopall;
  if(m_bNoMoreFiles) {
    MessageBox::information_message(this, tr("No more files to open."));
    m_bNoMoreFiles=false;
  }
}

void MainWindow::readFromStdout()                             //readFromStdout
{
  while(proc_jt9.canReadLine()) {
    auto line_read = proc_jt9.readLine ();
    if (auto p = std::strpbrk (line_read.constData (), "\n\r")) {
      // truncate before line ending chars
      line_read = line_read.left (p - line_read.constData ());
    }
    bool haveFSpread {false};
    float fSpread {0.};
    if (m_mode.startsWith ("FST4"))
      {
        auto text = line_read.mid (64, 6).trimmed ();
        if (text.size ())
          {
            fSpread = text.toFloat (&haveFSpread);
            line_read = line_read.left (64);
          }
        auto const& cs = m_config.my_callsign ().toLocal8Bit ();
        if ("FST4W" == m_mode && ui->cbNoOwnCall->isChecked ()
            && (line_read.contains (" " + cs + " ")
                || line_read.contains ("<" + cs + ">"))) {
          continue;
        }
      }
    if (m_mode!="FT8" and m_mode!="FT4" and !m_mode.startsWith ("FST4") and m_mode!="Q65") {
      //Pad 22-char msg to at least 37 chars
      line_read = line_read.left(44) + "              " + line_read.mid(44);
    }
    bool bAvgMsg=false;
    int navg=0;

    if(line_read.indexOf("<DecodeFinished>") >= 0) {
      m_bDecoded =  line_read.mid(20).trimmed().toInt() > 0;
      int n=line_read.trimmed().size();
      int n2=line_read.trimmed().mid(n-7).toInt();
      int n0=n2/1000;
      int n1=n2%1000;
      if(m_mode=="Q65") {
        ndecodes_label.setText(QString {"%1  %2"}.arg (n0).arg (n1));
      } else {
        if(m_nDecodes==0) ndecodes_label.setText("0");
      }
      decodeDone ();
      return;
    } else {
      m_nDecodes+=1;
      if(m_mode!="Q65") ndecodes_label.setText(QString::number(m_nDecodes));
      if(m_mode=="JT4" or m_mode=="JT65" or m_mode=="Q65") {
        //### Do something about Q65 here ?  ###
        int nf=line_read.indexOf("f");
        if(nf>0) {
          navg=line_read.mid(nf+1,1).toInt();
          if(line_read.indexOf("f*")>0) navg=10;
        }
        int nd=-1;
        if(nf<0) nd=line_read.indexOf("d");
        if(nd>0) {
          navg=line_read.mid(nd+2,1).toInt();
          if(line_read.mid(nd+2,1)=="*") navg=10;
        }
        int na=-1;
        if(nf<0 and nd<0) na=line_read.indexOf("a");
        if(na>0) {
          navg=line_read.mid(na+2,1).toInt();
          if(line_read.mid(na+2,1)=="*") navg=10;
        }
        int nq=-1;
        if(nf<0 and nd<0 and na<0) nq=line_read.indexOf("q");
        if(nq>0) {
          navg=line_read.mid(nq+2,1).toInt();
          if(line_read.mid(nq+2,1)=="*") navg=10;
        }
        if(navg>=2) bAvgMsg=true;
      }
      write_all("Rx",line_read.trimmed());
      int ntime=6;
      if(m_TRperiod>=60) ntime=4;
      if (line_read.left(ntime) != m_tBlankLine) {
          ui->decodedTextBrowser->new_period ();
          if (m_config.insert_blank ()
              && SpecOp::FOX != m_config.special_op_id()) {
            QString band;
            if((QDateTime::currentMSecsSinceEpoch() / 1000 - m_secBandChanged) > 4*int(m_TRperiod)/4) {
              band = ' ' + m_config.bands ()->find (m_freqNominal);
            }
            ui->decodedTextBrowser->insertLineSpacer (band.rightJustified  (40, '-'));
          }
        m_tBlankLine = line_read.left(ntime);
      }
      if ("FST4W" == m_mode)
        {
          uploadWSPRSpots (true, line_read);
        }
      DecodedText decodedtext0 {QString::fromUtf8(line_read.constData())};
      DecodedText decodedtext {QString::fromUtf8(line_read.constData()).remove("TU; ")};

      if(m_mode=="FT8" and SpecOp::FOX == m_config.special_op_id() and
         (decodedtext.string().contains("R+") or decodedtext.string().contains("R-"))) {
        auto for_us  = decodedtext.string().contains(" " + m_config.my_callsign() + " ") or
            decodedtext.string().contains(" "+m_baseCall) or
            decodedtext.string().contains(m_baseCall+" ") or
            decodedtext.string().contains(" <" + m_config.my_callsign() + "> ");
        if(decodedtext.string().contains(" DE ")) for_us=true;   //Hound with compound callsign
        if(for_us) {
          QString houndCall,houndGrid;
          decodedtext.deCallAndGrid(/*out*/houndCall,houndGrid);
          foxRxSequencer(decodedtext.string(),houndCall,houndGrid);
        }
      }

//Left (Band activity) window
      if(!bAvgMsg) {
        if(m_mode=="FT8" and SpecOp::FOX == m_config.special_op_id()) {
          if(!m_bDisplayedOnce) {
            // This hack sets the font.  Surely there's a better way!
            DecodedText dt{"."};
            ui->decodedTextBrowser->displayDecodedText (dt, m_config.my_callsign (), m_mode, m_config.DXCC (),
                m_logBook, m_currentBand, m_config.ppfx ());
            m_bDisplayedOnce=true;
          }
        } else {
          DecodedText decodedtext1=decodedtext0;
          ui->decodedTextBrowser->displayDecodedText (decodedtext1, m_config.my_callsign (), m_mode, m_config.DXCC (),
                                                      m_logBook, m_currentBand, m_config.ppfx (),
                                                      ui->cbCQonly->isVisible() && ui->cbCQonly->isChecked(),
                                                      haveFSpread, fSpread);

          if(m_bBestSPArmed && m_mode=="FT4" && CALLING == m_QSOProgress) {
            QString messagePriority=ui->decodedTextBrowser->CQPriority();
            if(messagePriority!="") {
              if(messagePriority=="New Call on Band"
                 and m_BestCQpriority!="New Call on Band"
                 and m_BestCQpriority!="New Multiplier") {
                m_BestCQpriority="New Call on Band";
                m_bDoubleClicked = true;
                processMessage(decodedtext0);
              }
              if(messagePriority=="New DXCC"
                 and m_BestCQpriority!="New DXCC"
                 and m_BestCQpriority!="New Multiplier") {
                m_BestCQpriority="New DXCC";
                m_bDoubleClicked = true;
                 processMessage(decodedtext0);
              }
            }
          }

        }
      }

//Right (Rx Frequency) window
      bool bDisplayRight=bAvgMsg;
      int audioFreq=decodedtext.frequencyOffset();
      if(m_mode=="FT8" or m_mode=="FT4" or m_mode=="FST4" or m_mode=="Q65") {
        int ftol=10;
        if(m_mode=="Q65") ftol=ui->sbFtol->value();
        auto const& parts = decodedtext.string().remove("<").remove(">")
            .split (' ', SkipEmptyParts);
        if (parts.size() > 6) {
          auto for_us = parts[5].contains (m_baseCall)
            || ("DE" == parts[5] && qAbs (ui->RxFreqSpinBox->value () - audioFreq) <= ftol);
          if(m_baseCall == m_config.my_callsign())
            {
              if (m_baseCall != parts[5])
                {
                  for_us=false;
                }
            }
          else
            {
              if (m_config.my_callsign () != parts[5])
                {
                  for_us = false; // same base call as ours but
                                  // different prefix or suffix, rare
                                  // but can happen with multi station
                                  // special events
                }
            }
          if(m_bCallingCQ && !m_bAutoReply && for_us && ui->cbFirst->isChecked() and
             SpecOp::FOX > m_config.special_op_id()) {
            m_bDoubleClicked=true;
            m_bAutoReply = true;
            // SP6XD: detect auto reply here
            // The easiest way (for now) is just to skip but we should 
            // move on to another reply (if any).
            if(SpecOp::FOX != m_config.special_op_id()) processMessage (decodedtext);
            ui->cbFirst->setStyleSheet("");
          }
 
          if(SpecOp::FOX==m_config.special_op_id() and decodedtext.string().contains(" DE ")) for_us=true; //Hound with compound callsign
          if(SpecOp::FOX==m_config.special_op_id() and for_us and (audioFreq<1000)) bDisplayRight=true;
          if(SpecOp::FOX!=m_config.special_op_id() and (for_us or (abs(audioFreq - m_wideGraph->rxFreq()) <= 10))) bDisplayRight=true;
        }
      } else {
        if((abs(audioFreq - m_wideGraph->rxFreq()) <= 10) and
           !m_config.enable_VHF_features()) bDisplayRight=true;
      }

      if (bDisplayRight) {
        // This msg is within 10 hertz of our tuned frequency, or a JT4 or JT65 avg,
        // or contains MyCall
        if(!m_bBestSPArmed or m_mode!="FT4") {
          ui->decodedTextBrowser2->displayDecodedText (decodedtext0, m_config.my_callsign (), m_mode, m_config.DXCC (),
                m_logBook, m_currentBand, m_config.ppfx ());
        }
        m_QSOText = decodedtext.string ().trimmed ();
      }

      postDecode (true, decodedtext.string ());

      if(m_mode=="FT8" and SpecOp::HOUND==m_config.special_op_id()) {
        if(decodedtext.string().contains(";")) {
          QStringList w=decodedtext.string().mid(24).split(" ",SkipEmptyParts);
          QString foxCall=w.at(3);
          foxCall=foxCall.remove("<").remove(">");
          if(w.at(0)==m_config.my_callsign() or w.at(0)==Radio::base_callsign(m_config.my_callsign())) {
            //### Check for ui->dxCallEntry->text()==foxCall before logging! ###
            ui->stopTxButton->click ();
            logQSOTimer.start(0);
          }
          if((w.at(2)==m_config.my_callsign() or w.at(2)==Radio::base_callsign(m_config.my_callsign()))
             and ui->tx3->text().length()>0) {
            m_rptRcvd=w.at(4);
            m_rptSent=decodedtext.string().mid(7,3);
            m_nFoxFreq=decodedtext.string().mid(16,4).toInt();
            hound_reply ();
          }
        } else {
          QStringList w=decodedtext.string().mid(24).split(" ",SkipEmptyParts);
          if(decodedtext.string().contains("/")) w.append(" +00");  //Add a dummy report
          if(w.size()>=3) {
            QString foxCall=w.at(1);
            if((w.at(0)==m_config.my_callsign() or w.at(0)==Radio::base_callsign(m_config.my_callsign())) and
               ui->tx3->text().length()>0) {
              if(w.at(2)=="RR73") {
                ui->stopTxButton->click ();
                logQSOTimer.start(0);
              } else {
                if(w.at(1)==Radio::base_callsign(ui->dxCallEntry->text()) and
                   (w.at(2).mid(0,1)=="+" or w.at(2).mid(0,1)=="-")) {
                  m_rptRcvd=w.at(2);
                  m_rptSent=decodedtext.string().mid(7,3);
                  m_nFoxFreq=decodedtext.string().mid(16,4).toInt();
                  hound_reply ();
                }
              }
            }
          }
        }
      }

//### I think this is where we are preventing Hounds from spotting Fox ###
      if(m_mode!="FT8" or (SpecOp::HOUND != m_config.special_op_id())) {
        if(m_mode=="FT8" or m_mode=="FT4" or m_mode=="Q65"
           or m_mode=="JT4" or m_mode=="JT65" or m_mode=="JT9" or m_mode=="FST4") {
          auto_sequence (decodedtext, 25, 50);
        }

// find and extract any report for myCall, but save in m_rptRcvd only if it's from DXcall
        QString rpt;
        bool stdMsg = decodedtext.report(m_baseCall,
            Radio::base_callsign(ui->dxCallEntry->text()), rpt);
        QString deCall;
        QString grid;
        decodedtext.deCallAndGrid(/*out*/deCall,grid);
        {
          auto t = Radio::base_callsign (ui->dxCallEntry->text ());
          auto const& dx_call = decodedtext.call ();
          if (rpt.size ()       // report in message
              && (m_baseCall == Radio::base_callsign (dx_call) // for us
                  || "DE" == dx_call)                          // probably for us
              && (t == deCall   // DX station base call is QSO partner
                  || ui->dxCallEntry->text () == deCall // DX station full call is QSO partner
                  || !t.size ()))                       // not in QSO
            {
              m_rptRcvd = rpt;
            }
        }
// extract details and send to PSKreporter
        int nsec=QDateTime::currentMSecsSinceEpoch()/1000-m_secBandChanged;
        bool okToPost=(nsec > int(4*m_TRperiod)/5);
        if(m_mode=="FST4W" and okToPost) {
          line_read=line_read.left(22) + " CQ " + line_read.trimmed().mid(22);
          auto p = line_read.lastIndexOf (' ');
          DecodedText FST4W_post {QString::fromUtf8 (line_read.left (p).constData ())};
          pskPost(FST4W_post);
        } else {
          if (stdMsg && okToPost) pskPost(decodedtext);
        }
        if((m_mode=="JT4" or m_mode=="JT65" or m_mode=="Q65") and
           m_msgAvgWidget!=NULL) {
          if(m_msgAvgWidget->isVisible()) {
            QFile f(m_config.temp_dir ().absoluteFilePath ("avemsg.txt"));
            if(f.open(QIODevice::ReadOnly | QIODevice::Text)) {
              QTextStream s(&f);
              QString t=s.readAll();
              m_msgAvgWidget->displayAvg(t);
            }
          }
        }
      }
    }
  }
}

//
// start_tolerance - only respond to "DE ..." and free text 73
//                   messages within +/- this value
//
// stop_tolerance - kill Tx if running station is seen to reply to
//                  another caller and we are going to transmit within
//                  +/- this value of the reply to another caller
//
void MainWindow::auto_sequence (DecodedText const& message, unsigned start_tolerance, unsigned stop_tolerance)
{
  auto const& message_words = message.messageWords ();
  auto is_73 = message_words.filter (QRegularExpression {"^(73|RR73)$"}).size();
  auto msg_no_hash = message.clean_string();
  msg_no_hash = msg_no_hash.mid(22).remove("<").remove(">");
  bool is_OK=false;
  if(m_mode=="MSK144" && msg_no_hash.indexOf(ui->dxCallEntry->text()+" R ")>0) is_OK=true;
  if (message_words.size () > 3 && (message.isStandardMessage() || (is_73 or is_OK))) {
    auto df = message.frequencyOffset ();
    auto within_tolerance = (qAbs (ui->RxFreqSpinBox->value () - df) <= int (start_tolerance)
       || qAbs (ui->TxFreqSpinBox->value () - df) <= int (start_tolerance));
    bool acceptable_73 = is_73
      && m_QSOProgress >= ROGER_REPORT
      && ((message.isStandardMessage ()
           && (message_words.contains (m_baseCall)
               || message_words.contains (m_config.my_callsign ())
               || message_words.contains (ui->dxCallEntry->text ())
               || message_words.contains (Radio::base_callsign (ui->dxCallEntry->text ()))
               || message_words.contains ("DE")))
          || !message.isStandardMessage ()); // free text 73/RR73

    auto const& w = msg_no_hash.split(" ",SkipEmptyParts);
    QString w2;
    int nrpt=0;
    if (w.size () > 2)
      {
        w2=w.at(2);
        if(w.size()>3) {
          nrpt=w2.toInt();
          if(w2=="R") nrpt=w.at(3).toInt();
        }
      }
    bool bEU_VHF=(nrpt>=520001 and nrpt<=594000);
    if(bEU_VHF and message.clean_string ().contains("<"+m_config.my_callsign() + "> ")) {
      m_xRcvd=message.clean_string ().trimmed().right(13);
    }
    if (m_auto
        && (m_QSOProgress==REPLYING  or (!ui->tx1->isEnabled () and m_QSOProgress==REPORT))
        && qAbs (ui->TxFreqSpinBox->value () - df) <= int (stop_tolerance)
        && message_words.at (2) != "DE"
        && !message_words.at (2).contains (QRegularExpression {"(^(CQ|QRZ))|" + m_baseCall})
        && message_words.at (3).contains (Radio::base_callsign (ui->dxCallEntry->text ()))) {
      // auto stop to avoid accidental QRM
      ui->stopTxButton->click (); // halt any transmission
    } else if (m_auto             // transmit allowed
               && ui->cbAutoSeq->isVisible () && ui->cbAutoSeq->isEnabled () && ui->cbAutoSeq->isChecked () // auto-sequencing allowed
               && ((!m_bCallingCQ      // not calling CQ/QRZ
                    && !m_sentFirst73       // not finished QSO
                    && ((message_words.at (2).contains (m_baseCall)
                         // being called and not already in a QSO
                         && (message_words.at(3).contains(Radio::base_callsign(ui->dxCallEntry->text()))
                             or bEU_VHF))
                        || message_words.at(1) == m_baseCall // <de-call> RR73; ...
                        // type 2 compound replies
                        || (within_tolerance &&
                            (acceptable_73 ||
                             ("DE" == message_words.at (2) &&
                              w2.contains(Radio::base_callsign (m_hisCall)))))))
                   || (m_bCallingCQ && m_bAutoReply
                       // look for type 2 compound call replies on our Tx and Rx offsets
                       && ((within_tolerance && "DE" == message_words.at (2))
                           || message_words.at (2).contains (m_baseCall))))) {
      if(SpecOp::FOX != m_config.special_op_id()) processMessage (message);
    }
  }
}

void MainWindow::pskPost (DecodedText const& decodedtext)
{
  if (m_diskData || !m_config.spot_to_psk_reporter() || decodedtext.isLowConfidence ()) return;

  QString msgmode=m_mode;
  QString deCall;
  QString grid;
  decodedtext.deCallAndGrid(/*out*/deCall,grid);
  int audioFrequency = decodedtext.frequencyOffset();
  if(m_mode=="FT8" or m_mode=="MSK144" or m_mode=="FT4") {
    audioFrequency=decodedtext.string().mid(16,4).toInt();
  }
  int snr = decodedtext.snr();
  Frequency frequency = m_freqNominal + audioFrequency;
  if(grid.contains (grid_regexp)) {
//    qDebug() << "To PSKreporter:" << deCall << grid << frequency << msgmode << snr;
    if (!m_psk_Reporter.addRemoteStation (deCall, grid, frequency, msgmode, snr))
      {
        showStatusMessage (tr ("Spotting to PSK Reporter unavailable"));
      }
  }
}

void MainWindow::killFile ()
{
//  qDebug() << "cc 3725" << m_saveDecoded << m_saveAll << m_bDecoded << m_nDecodes << m_fnameWE;
  if (m_fnameWE.size () && !(m_saveAll || (m_saveDecoded && m_bDecoded))) {
    QFile f1 {m_fnameWE + ".wav"};
    if(f1.exists()) f1.remove();
    if(m_mode=="WSPR" or m_mode=="FST4W") {
      QFile f2 {m_fnameWE + ".c2"};
      if(f2.exists()) f2.remove();
    }
  }
}

void MainWindow::on_EraseButton_clicked ()
{
  qint64 ms=QDateTime::currentMSecsSinceEpoch();
  ui->decodedTextBrowser2->erase ();
  if(m_mode=="WSPR" or m_mode=="Echo" or m_mode=="FST4W") {
    ui->decodedTextBrowser->erase ();
  } else {
    if((ms-m_msErase)<500) {
      ui->decodedTextBrowser->erase ();
    }
  }
  m_msErase=ms;
}

void MainWindow::band_activity_cleared ()
{
  m_messageClient->decodes_cleared ();
  QFile f(m_config.temp_dir ().absoluteFilePath ("decoded.txt"));
  if(f.exists()) f.remove();
}

void MainWindow::rx_frequency_activity_cleared ()
{
  m_QSOText.clear();
  set_dateTimeQSO(-1);          // G4WJS: why do we do this?
}

void MainWindow::decodeBusy(bool b)                             //decodeBusy()
{
  if (!b) {
    m_optimizingProgress.reset ();
  }
  m_decoderBusy=b;
  ui->DecodeButton->setEnabled(!b);
  ui->actionOpen->setEnabled(!b);
  ui->actionOpen_next_in_directory->setEnabled(!b);
  ui->actionDecode_remaining_files_in_directory->setEnabled(!b);

  statusUpdate ();
}

//------------------------------------------------------------- //guiUpdate()
void MainWindow::guiUpdate()
{
  static char message[38];
  static char msgsent[38];
  double txDuration;

  if(m_TRperiod==0) m_TRperiod=60.0;
  txDuration=tx_duration(m_mode,m_TRperiod,m_nsps,m_bFast9);
  double tx1=0.0;
  double tx2=txDuration;
  if(m_mode=="FT8" or m_mode=="FT4") icw[0]=0;              //No CW ID in FT4 or FT8 mode
  if((icw[0]>0) and (!m_bFast9)) tx2 += icw[0]*2560.0/48000.0;  //Full length including CW ID
  if(tx2>m_TRperiod) tx2=m_TRperiod;
  if(!m_txFirst and m_mode!="WSPR" and m_mode!="FST4W") {
    tx1 += m_TRperiod;
    tx2 += m_TRperiod;
  }

  qint64 ms = QDateTime::currentMSecsSinceEpoch() % 86400000;
  int nsec=ms/1000;
  double tsec=0.001*ms;
  double t2p=fmod(tsec,2*m_TRperiod);
  double s6=fmod(tsec,6.0);
  int nseq = fmod(double(nsec),m_TRperiod);
  m_tRemaining=m_TRperiod - fmod(tsec,m_TRperiod);

  if(m_mode=="Echo") {
    tx1=0.0;
    tx2=txDuration;
    if(m_auto and s6>4.0) m_bEchoTxOK=true;
    if(m_transmitting) m_bEchoTxed=true;
  }

  if(m_mode=="WSPR" or m_mode=="FST4W") {
    if(nseq==0 and m_ntr==0) {                   //Decide whether to Tx or Rx
      m_tuneup=false;                              //This is not an ATU tuneup
      bool btx = m_auto && m_WSPR_tx_next;         // To Tx, we need m_auto and
                                                   // scheduled transmit
      m_WSPR_tx_next = false;
      if(btx) {
        m_ntr=-1;                                  //This says we will have transmitted
        ui->pbTxNext->setChecked (false);
        m_bTxTime=true;                            //Start a WSPR or FST4W Tx sequence
      } else {
        // This will be a WSPR or FST4W Rx sequence.
        m_ntr=1;                                   //This says we will have received
        m_bTxTime=false;                           //Start a WSPR or FST4W Rx sequence
      }
    }

  } else {
    // For all modes other than WSPR and FST4W
    m_bTxTime = (t2p >= tx1) and (t2p < tx2);
    if(m_mode=="Echo") m_bTxTime = m_bTxTime and m_bEchoTxOK;
    if(m_mode=="FT8" and ui->tx5->currentText().contains("/B ")) {
      //FT8 beacon transmissiion from Tx5 only at top of a UTC minute
      double t4p=fmod(tsec,4*m_TRperiod);
      if(t4p >= 30.0) m_bTxTime=false;
    }
  }
  if(m_tune) m_bTxTime=true;                 //"Tune" takes precedence

  if(m_transmitting or m_auto or m_tune) {
    m_dateTimeLastTX = QDateTime::currentDateTimeUtc ();

// Check for "txboth" (FT4 testing purposes only)
    QFile f(m_appDir + "/txboth");
    if(f.exists() and fmod(tsec,m_TRperiod) < (0.5 + 105.0*576.0/12000.0)) m_bTxTime=true;

// Don't transmit another mode in the 30 m WSPR sub-band
    Frequency onAirFreq = m_freqNominal + ui->TxFreqSpinBox->value();
    if ((onAirFreq > 10139900 and onAirFreq < 10140320) and m_mode!="WSPR") {
      m_bTxTime=false;
      if (m_auto) auto_tx_mode (false);
      if(onAirFreq!=m_onAirFreq0) {
        m_onAirFreq0=onAirFreq;
        auto const& message = tr ("Please choose another Tx frequency."
                                  " WSJT-X will not knowingly transmit another"
                                  " mode in the WSPR sub-band on 30m.");
        QTimer::singleShot (0, [=] { // don't block guiUpdate
            MessageBox::warning_message (this, tr ("WSPR Guard Band"), message);
          });
      }
    }

    if(m_mode=="FT8" and SpecOp::FOX==m_config.special_op_id()) {
// Don't allow Fox mode in any of the default FT8 sub-bands.
      qint32 ft8Freq[]={1840,3573,7074,10136,14074,18100,21074,24915,28074,50313,70100};
      for(int i=0; i<11; i++) {
        int kHzdiff=m_freqNominal/1000 - ft8Freq[i];
        if(qAbs(kHzdiff) < 4) {
          m_bTxTime=false;
          if (m_auto) auto_tx_mode (false);
          auto const& message = tr ("Please choose another dial frequency."
                                    " WSJT-X will not operate in Fox mode"
                                    " in the standard FT8 sub-bands.");
          QTimer::singleShot (0, [=] {               // don't block guiUpdate
            MessageBox::warning_message (this, tr ("Fox Mode warning"), message);
          });
          break;
        }
      }
    }

    if (m_config.watchdog() && m_mode!="WSPR" && m_mode!="FST4W"
        && m_idleMinutes >= m_config.watchdog ()) {
      tx_watchdog (true);       // disable transmit
    }

    double fTR=float((ms%int(1000.0*m_TRperiod)))/int(1000.0*m_TRperiod);

    QString txMsg;
    if(m_ntx == 1) txMsg=ui->tx1->text();
    if(m_ntx == 2) txMsg=ui->tx2->text();
    if(m_ntx == 3) txMsg=ui->tx3->text();
    if(m_ntx == 4) txMsg=ui->tx4->text();
    if(m_ntx == 5) txMsg=ui->tx5->currentText();
    if(m_ntx == 6) txMsg=ui->tx6->text();
    int msgLength=txMsg.trimmed().length();
    if(msgLength==0 and !m_tune) on_stopTxButton_clicked();

    if(g_iptt==0 and ((m_bTxTime and (fTR < 0.75) and (msgLength>0)) or m_tune)) {
      //### Allow late starts
      icw[0]=m_ncw;
      g_iptt = 1;
      setRig ();
      if(m_mode=="FT8") {
        if (SpecOp::FOX == m_config.special_op_id()) {
          if (ui->TxFreqSpinBox->value() > 900) {
            ui->TxFreqSpinBox->setValue(300);
          }
        }
        else if (SpecOp::HOUND == m_config.special_op_id()) {
          if(m_auto && !m_tune) {
            if (ui->TxFreqSpinBox->value() < 999 && m_ntx != 3) {
              // Hound randomized range: 1000-3000 Hz
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
              ui->TxFreqSpinBox->setValue (QRandomGenerator::global ()->bounded (1000, 2999));
#else
              ui->TxFreqSpinBox->setValue ((qrand () % 2000) + 1000);
#endif
            }
          }
          if (m_nSentFoxRrpt==2 and m_ntx==3) {
            // move off the original Fox frequency on subsequent tries of Tx3
            int nfreq=m_nFoxFreq + 300;
            if(m_nFoxFreq>600) nfreq=m_nFoxFreq - 300;  //keep nfreq below 900 Hz
            ui->TxFreqSpinBox->setValue(nfreq);
          }
          if (m_nSentFoxRrpt == 1) {
            ++m_nSentFoxRrpt;
          }
        }
      }
      

// If HoldTxFreq is not checked, randomize Fox's Tx Freq
// NB: Maybe this should be done no more than once every 5 minutes or so ?
      if(m_mode=="FT8" and SpecOp::FOX==m_config.special_op_id() and !ui->cbHoldTxFreq->isChecked()) {
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        ui->TxFreqSpinBox->setValue (QRandomGenerator::global ()->bounded (300, 599));
#else
        ui->TxFreqSpinBox->setValue(300.0 + 300.0*double(qrand())/RAND_MAX);
#endif
      }

      setXIT (ui->TxFreqSpinBox->value ());
      m_config.transceiver_ptt (true); //Assert the PTT
      m_tx_when_ready = true;
    }
//    if(!m_bTxTime and !m_tune and m_mode!="FT4") m_btxok=false;       //Time to stop transmitting
    if(!m_bTxTime and !m_tune) m_btxok=false;       //Time to stop transmitting
  }

  if((m_mode=="WSPR" or m_mode=="FST4W") and
     ((m_ntr==1 and m_rxDone) or (m_ntr==-1 and nseq>tx2))) {
    if(m_monitoring) {
      m_rxDone=false;
    }
    if(m_transmitting) {
      WSPR_history(m_freqNominal,-1);
      m_bTxTime=false;                        //Time to stop a WSPR or FST4W transmission
      m_btxok=false;
    }
    else if (m_ntr != -1) {
      WSPR_scheduling ();
      m_ntr=0;                                //This WSPR or FST4W Rx sequence is complete
    }
  }


  // Calculate Tx tones when needed
  if((g_iptt==1 && m_iptt0==0) || m_restart) {
//----------------------------------------------------------------------
    QByteArray ba;
    QByteArray ba0;

    if(m_mode=="WSPR") {
      ba=WSPR_message().toLatin1();
    } else {
      if(SpecOp::HOUND == m_config.special_op_id() and m_ntx!=3) {   //Hound transmits only Tx1 or Tx3
        m_ntx=1;
        ui->txrb1->setChecked(true);
      }

      if(m_mode=="FT4" and m_bBestSPArmed) {
        m_BestCQpriority="";
        m_bBestSPArmed=false;
        ui->pbBestSP->setStyleSheet ("");
      }

      if(m_ntx == 1) ba=ui->tx1->text().toLocal8Bit();
      if(m_ntx == 2) ba=ui->tx2->text().toLocal8Bit();
      if(m_ntx == 3) ba=ui->tx3->text().toLocal8Bit();
      if(m_ntx == 4) ba=ui->tx4->text().toLocal8Bit();
      if(m_ntx == 5) ba=ui->tx5->currentText().toLocal8Bit();
      if(m_ntx == 6) ba=ui->tx6->text().toLocal8Bit();
    }

    ba2msg(ba,message);
    int ichk=0;
    if (m_lastMessageSent != m_currentMessage
        || m_lastMessageType != m_currentMessageType)
      {
        m_lastMessageSent = m_currentMessage;
        m_lastMessageType = m_currentMessageType;
      }
    m_currentMessageType = 0;
    if(m_tune or m_mode=="Echo") {
      itone[0]=0;
    } else {
      if(m_QSOProgress==REPORT || m_QSOProgress==ROGER_REPORT) m_bSentReport=true;
      if(m_bSentReport and (m_QSOProgress<REPORT or m_QSOProgress>ROGER_REPORT)) m_bSentReport=false;
      if(m_mode=="JT4") gen4_(message, &ichk , msgsent, const_cast<int *> (itone),
                                &m_currentMessageType, 22, 22);
      if(m_mode=="JT9") gen9_(message, &ichk, msgsent, const_cast<int *> (itone),
                                &m_currentMessageType, 22, 22);
      if(m_mode=="JT65") gen65_(message, &ichk, msgsent, const_cast<int *> (itone),
                                  &m_currentMessageType, 22, 22);
      if(m_mode=="WSPR") genwspr_(message, msgsent, const_cast<int *> (itone),
                                    22, 22);
      if(m_mode=="MSK144" or m_mode=="FT8" or m_mode=="FT4"
         or m_mode=="FST4" or m_mode=="FST4W" || "Q65" == m_mode) {
        if(m_mode=="MSK144") {
          genmsk_128_90_(message, &ichk, msgsent, const_cast<int *> (itone),
                         &m_currentMessageType, 37, 37);
          if(m_restart) {
            int nsym=144;
            if(itone[40]==-40) nsym=40;
            m_modulator->set_nsym(nsym);
          }
        }

        if(m_mode=="FT8") {
          if(SpecOp::FOX==m_config.special_op_id() and ui->tabWidget->currentIndex()==1) {
            foxTxSequencer();
          } else {
            int i3=0;
            int n3=0;
            char ft8msgbits[77];
            genft8_(message, &i3, &n3, msgsent, const_cast<char *> (ft8msgbits),
                    const_cast<int *> (itone), 37, 37);
            int nsym=79;
            int nsps=4*1920;
            float fsample=48000.0;
            float bt=2.0;
            float f0=ui->TxFreqSpinBox->value() - m_XIT;
            int icmplx=0;
            int nwave=nsym*nsps;
            gen_ft8wave_(const_cast<int *>(itone),&nsym,&nsps,&bt,&fsample,&f0,foxcom_.wave,
                         foxcom_.wave,&icmplx,&nwave);
            if(SpecOp::FOX == m_config.special_op_id()) {
              //Fox must generate the full Tx waveform, not just an itone[] array.
              QString fm = QString::fromStdString(message).trimmed();
              foxGenWaveform(0,fm);
              foxcom_.nslots=1;
              foxcom_.nfreq=ui->TxFreqSpinBox->value();
              if(m_config.split_mode()) foxcom_.nfreq = foxcom_.nfreq - m_XIT;  //Fox Tx freq
              QString foxCall=m_config.my_callsign() + "         ";
              ::memcpy(foxcom_.mycall, foxCall.toLatin1(), sizeof foxcom_.mycall); //Copy Fox callsign into foxcom_
              foxgen_();
            }
          }
        }
        if(m_mode=="FT4") {
          int ichk=0;
          char ft4msgbits[77];
          genft4_(message, &ichk, msgsent, const_cast<char *> (ft4msgbits),
                  const_cast<int *>(itone), 37, 37);
          int nsym=103;
          int nsps=4*576;
          float fsample=48000.0;
          float f0=ui->TxFreqSpinBox->value() - m_XIT;
          int nwave=(nsym+2)*nsps;
          int icmplx=0;
          gen_ft4wave_(const_cast<int *>(itone),&nsym,&nsps,&fsample,&f0,foxcom_.wave,
                       foxcom_.wave,&icmplx,&nwave);
        }
        if(m_mode=="FST4" or m_mode=="FST4W") {
          int ichk=0;
          int iwspr=0;
          char fst4msgbits[101];
          QString wmsg;
          if(m_mode=="FST4W") {
            iwspr = 1;
            wmsg=WSPR_message();
            ba=wmsg.toLatin1();
            ba2msg(ba,message);
          }
          genfst4_(message,&ichk,msgsent,const_cast<char *> (fst4msgbits),
                   const_cast<int *>(itone), &iwspr, 37, 37);
          int hmod=1;
          if(m_config.x2ToneSpacing()) hmod=2;
          if(m_config.x4ToneSpacing()) hmod=4;
          int nsps=720;
          if(m_TRperiod==30) nsps=1680;
          if(m_TRperiod==60) nsps=3888;
          if(m_TRperiod==120) nsps=8200;
          if(m_TRperiod==300) nsps=21504;
          if(m_TRperiod==900) nsps=66560;
          if(m_TRperiod==1800) nsps=134400;
          nsps=4*nsps;                           //48000 Hz sampling
          int nsym=160;
          float fsample=48000.0;
          float dfreq=hmod*fsample/nsps;
          float f0=ui->TxFreqSpinBox->value() - m_XIT + 1.5*dfreq;
          if(m_mode=="FST4W") f0=ui->WSPRfreqSpinBox->value() - m_XIT + 1.5*dfreq;
          int nwave=(nsym+2)*nsps;
          int icmplx=0;
          gen_fst4wave_(const_cast<int *>(itone),&nsym,&nsps,&nwave,
                        &fsample,&hmod,&f0,&icmplx,foxcom_.wave,foxcom_.wave);

          QString t = QString::fromStdString(message).trimmed();
        }
        if(m_mode=="Q65") {
          int i3=-1;
          int n3=-1;
          genq65_(message,&ichk,msgsent,const_cast<int *>(itone),&i3,&n3,37,37);
          int nsps=1800;
          if(m_TRperiod==30) nsps=3600;
          if(m_TRperiod==60) nsps=7200;
          if(m_TRperiod==120) nsps=16000;
          if(m_TRperiod==300) nsps=41472;
          int nsps4=4*nsps;                           //48000 Hz sampling
          int nsym=85;
          float fsample=48000.0;
          int nwave=(nsym+2)*nsps4;
          int icmplx=0;
          int hmod=1;
          float f0=ui->TxFreqSpinBox->value()-m_XIT;
          genwave_(const_cast<int *>(itone),&nsym,&nsps4,&nwave,
                   &fsample,&hmod,&f0,&icmplx,foxcom_.wave,foxcom_.wave);
        }

        if(SpecOp::EU_VHF==m_config.special_op_id()) {
          if(m_ntx==2) m_xSent=ui->tx2->text().right(13);
          if(m_ntx==3) m_xSent=ui->tx3->text().right(13);
        }

        if(SpecOp::FIELD_DAY==m_config.special_op_id() or SpecOp::RTTY==m_config.special_op_id()) {
          if(m_ntx==2 or m_ntx==3) {
            QStringList t=ui->tx2->text().split(' ', SkipEmptyParts);
            int n=t.size();
            m_xSent=t.at(n-2) + " " + t.at(n-1);
          }
        }
      }
      msgsent[37]=0;
    }

    {
      auto temp = m_currentMessage;
      m_currentMessage = QString::fromLatin1(msgsent);
      if (m_currentMessage != temp) // check if tx message changed
      {
          statusUpdate ();
        }
    }
    m_bCallingCQ = 6 == m_ntx
      || m_currentMessage.contains (QRegularExpression {"^(CQ|QRZ) "});
    if(m_mode=="FT8" or m_mode=="FT4") {
      if(m_bCallingCQ && ui->cbFirst->isVisible () && ui->cbFirst->isChecked ()) {
        ui->cbFirst->setStyleSheet("QCheckBox{color:red}");
      } else {
        ui->cbFirst->setStyleSheet("");
      }
    }

    if (m_tune) {
      m_currentMessage = "TUNE";
      m_currentMessageType = -1;
    }
    if(m_restart) {
      write_all("Tx",m_currentMessage);
      if (m_config.TX_messages ()) {
        ui->decodedTextBrowser2->displayTransmittedText(m_currentMessage.trimmed(),m_mode,
                     ui->TxFreqSpinBox->value(),m_bFastMode,m_TRperiod);
        }
    }

    auto t2 = QDateTime::currentDateTimeUtc ().toString ("hhmm");
    icw[0] = 0;
    auto msg_parts = m_currentMessage.split (' ', SkipEmptyParts);
    if (msg_parts.size () > 2) {
      // clean up short code forms
      msg_parts[0].remove (QChar {'<'});
      msg_parts[0].remove (QChar {'>'});
      msg_parts[1].remove (QChar {'<'});
      msg_parts[1].remove (QChar {'>'});
    }
    auto is_73 = message_is_73 (m_currentMessageType, msg_parts);
    m_sentFirst73 = is_73
      && !message_is_73 (m_lastMessageType, m_lastMessageSent.split (' ', SkipEmptyParts));
    if (m_sentFirst73 || (is_73 && CALLING == m_QSOProgress)) {
      m_qsoStop=t2;
      if(m_config.id_after_73 ()) {
        icw[0] = m_ncw;
      }
      if((m_config.prompt_to_log() or m_config.autoLog()) && !m_tune && CALLING != m_QSOProgress)
        {
          logQSOTimer.start(0);
        }
      else
        {
          cease_auto_Tx_after_QSO ();
        }
    }

    bool b=("FT8"==m_mode or "FT4"==m_mode or "Q65"==m_mode) and ui->cbAutoSeq->isVisible () && ui->cbAutoSeq->isEnabled () && ui->cbAutoSeq->isChecked ();
    if(is_73 and (m_config.disable_TX_on_73() or b)) {
      m_nextCall="";  //### Temporary: disable use of "TU;" messages;
      if(m_nextCall!="") {
        useNextCall();
      } else {
        auto_tx_mode (false);
        if(b) {
          m_ntx=6;
          ui->txrb6->setChecked(true);
          m_QSOProgress = CALLING;
        }
      }
    }

    if(m_config.id_interval () >0) {
      int nmin=(m_sec0-m_secID)/60;
      if(m_sec0<m_secID) nmin=m_config.id_interval();
      if(nmin >= m_config.id_interval()) {
        icw[0]=m_ncw;
        m_secID=m_sec0;
      }
    }

    if ((m_currentMessageType < 6 || 7 == m_currentMessageType)
        && msg_parts.length() >= 3
        && (msg_parts[1] == m_config.my_callsign () ||
            msg_parts[1] == m_baseCall))
    {
      int i1;
      bool ok;
      i1 = msg_parts[2].toInt(&ok);
      if(ok and i1>=-50 and i1<50)
      {
        m_rptSent = msg_parts[2];
        m_qsoStart = t2;
      } else {
        if (msg_parts[2].mid (0, 1) == "R")
        {
          i1 = msg_parts[2].mid (1).toInt (&ok);
          if (ok and i1 >= -50 and i1 < 50)
          {
            m_rptSent = msg_parts[2].mid (1);
            m_qsoStart = t2;
          }
        }
      }
    }
    m_restart=false;
//----------------------------------------------------------------------
  } else {
    if (!m_auto && m_sentFirst73) {
      m_sentFirst73 = false;
    }
  }

  if (g_iptt == 1 && m_iptt0 == 0) {
    auto const& current_message = QString::fromLatin1 (msgsent);
    if(m_config.watchdog () && m_mode!="WSPR" && m_mode!="FST4W"
       && current_message != m_msgSent0) {
      tx_watchdog (false);  // in case we are auto sequencing
      m_msgSent0 = current_message;
    }

    if (m_mode != "FST4W" && m_mode != "WSPR")
      {
        if(!m_tune) write_all("Tx",m_currentMessage);
        if (m_config.TX_messages () && !m_tune && SpecOp::FOX!=m_config.special_op_id())
          {
            ui->decodedTextBrowser2->displayTransmittedText(current_message.trimmed(),
                  m_mode,ui->TxFreqSpinBox->value(),m_bFastMode,m_TRperiod);
          }
      }

    switch (m_ntx)
    {
      case 1: m_QSOProgress = REPLYING; break;
      case 2: m_QSOProgress = REPORT; break;
      case 3: m_QSOProgress = ROGER_REPORT; break;
      case 4: m_QSOProgress = ROGERS; break;
      case 5: m_QSOProgress = SIGNOFF; break;
      case 6: m_QSOProgress = CALLING; break;
      default: break;             // determined elsewhere
    }
    m_transmitting = true;
    transmitDisplay (true);
    statusUpdate ();
  }

  if(!m_btxok && m_btxok0 && g_iptt==1) {
    stopTx();
    if ("1" == m_env.value ("WSJT_TX_BOTH", "0")) {
      m_txFirst = !m_txFirst;
      ui->txFirstCheckBox->setChecked (m_txFirst);
    }
  }

  if(m_startAnother) {
    if(m_mode=="MSK144") {
      m_wait++;
    }
    if(m_mode!="MSK144" or m_wait>=4) {
      m_wait=0;
      m_startAnother=false;
      on_actionOpen_next_in_directory_triggered();
    }
  }

  if(m_mode=="FT8" or m_mode=="MSK144" or m_mode=="FT4" || "Q65" == m_mode) {
    if(ui->txrb1->isEnabled() and
       (SpecOp::NA_VHF==m_config.special_op_id() or
        SpecOp::FIELD_DAY==m_config.special_op_id() or
        SpecOp::RTTY==m_config.special_op_id() or
        SpecOp::WW_DIGI==m_config.special_op_id()) ) {
      //We're in a contest-like mode other than EU_VHF: start QSO with Tx2.
      ui->tx1->setEnabled(false);
      ui->txb1->setEnabled(false);
    }
    if(!ui->tx1->isEnabled() and SpecOp::EU_VHF==m_config.special_op_id()) {
      //We're in EU_VHF mode: start QSO with Tx1.
      ui->tx1->setEnabled(true);
      ui->txb1->setEnabled(true);
    }
  }

//Once per second (onesec)
  if(nsec != m_sec0) {
//    qDebug() << "AAA" << nsec;

    if(m_mode=="FST4") chk_FST4_freq_range();
    m_currentBand=m_config.bands()->find(m_freqNominal);
    if( SpecOp::HOUND == m_config.special_op_id() ) {
      qint32 tHound=QDateTime::currentMSecsSinceEpoch()/1000 - m_tAutoOn;
      //To keep calling Fox, Hound must reactivate Enable Tx at least once every 2 minutes
      if(tHound >= 120 and m_ntx==1) auto_tx_mode(false);
    }

    progressBar.setVisible(true);
    progressBar.setFormat ("%v/%m");
    if(m_auto and m_mode=="Echo" and m_bEchoTxOK) {
      progressBar.setMaximum(3);
      progressBar.setValue(int(s6));
    }
    if(m_mode!="Echo") {
      if(m_monitoring or m_transmitting) {
        progressBar.setMaximum(m_TRperiod);
        int isec=int(fmod(tsec,m_TRperiod));
        if(m_TRperiod-int(m_TRperiod)>0.0) {
          QString progBarLabel;
          progBarLabel = progBarLabel.asprintf("%d/%3.1f",isec,m_TRperiod);
          progressBar.setFormat (progBarLabel);
        }
        progressBar.setValue(isec);
      } else {
        progressBar.setValue(0);
      }
    }

    astroUpdate ();

    if(m_transmitting) {
      char s[42];
      if(SpecOp::FOX==m_config.special_op_id() and ui->tabWidget->currentIndex()==1) {
        sprintf(s,"Tx:  %d Slots",foxcom_.nslots);
      } else {
        sprintf(s,"Tx: %s",msgsent);
      }
      m_nsendingsh=0;
      if(s[4]==64) m_nsendingsh=1;
      if(m_nsendingsh==1 or m_currentMessageType==7) {
        tx_status_label.setStyleSheet("QLabel{color: #000000; background-color: #66ffff}");
      } else if(m_nsendingsh==-1 or m_currentMessageType==6) {
        tx_status_label.setStyleSheet("QLabel{color: #000000; background-color: #ffccff}");
      } else {
        tx_status_label.setStyleSheet("QLabel{color: #000000; background-color: #ffff33}");
      }
      if(m_tune) {
        tx_status_label.setText("Tx: TUNE");
      } else {
        if(m_mode=="Echo") {
          tx_status_label.setText("Tx: ECHO");
        } else {
          s[40]=0;
          QString t{QString::fromLatin1(s)};
          if(SpecOp::FOX==m_config.special_op_id() and ui->tabWidget->currentIndex()==1 and foxcom_.nslots==1) {
              t=m_fm1.trimmed();
          }
          if(m_mode=="FT4") t="Tx: "+ m_currentMessage;
          tx_status_label.setText(t.trimmed());
        }
      }
    } else if(m_monitoring) {
      if (!m_tx_watchdog) {
        tx_status_label.setStyleSheet("QLabel{color: #000000; background-color: #00ff00}");
        auto t = tr ("Receiving");
        if(m_mode=="MSK144") {
          int npct=int(100.0*m_fCPUmskrtd/0.298667);
          if(npct>90) tx_status_label.setStyleSheet("QLabel{color: #000000; background-color: #ff0000}");
          t += QString {"   %1%"}.arg (npct, 2);
        }
        tx_status_label.setText (t);
      }
      transmitDisplay(false);
    } else if (!m_diskData && !m_tx_watchdog) {
      tx_status_label.setStyleSheet("");
      tx_status_label.setText("");
    }

    QDateTime t = QDateTime::currentDateTimeUtc();
    QString utc = t.date().toString("yyyy MMM dd") + "\n " +
      t.time().toString() + " ";
    ui->labUTC->setText(utc);
    if(m_bBestSPArmed and (m_dateTimeBestSP.secsTo(t) >= 120)) on_pbBestSP_clicked(); //BestSP timeout
    if(!m_monitoring and !m_diskData) ui->signal_meter_widget->setValue(0,0);
    m_sec0=nsec;
    displayDialFrequency ();
  }
  m_iptt0=g_iptt;
  m_btxok0=m_btxok;
}               //End of guiUpdate

void MainWindow::useNextCall()
{
  ui->dxCallEntry->setText(m_nextCall);
  m_nextCall="";
  if(m_nextGrid.contains(grid_regexp)) {
    ui->dxGridEntry->setText(m_nextGrid);
    m_ntx=2;
    ui->txrb2->setChecked(true);
  } else {
    m_ntx=3;
    ui->txrb3->setChecked(true);
  }
  genStdMsgs(m_nextRpt);
}

void MainWindow::startTx2()
{
  if (!m_modulator->isActive ()) { // TODO - not thread safe
    double fSpread=0.0;
    double snr=99.0;
    QString t=ui->tx5->currentText();
    if(t.mid(0,1)=="#") fSpread=t.mid(1,5).toDouble();
    m_modulator->setSpread(fSpread); // TODO - not thread safe
    t=ui->tx6->text();
    if(t.mid(0,1)=="#") snr=t.mid(1,5).toDouble();
    if(snr>0.0 or snr < -50.0) snr=99.0;
    if((m_ntx==6 or m_ntx==7) and m_config.force_call_1st()) {
      ui->cbAutoSeq->setChecked(true);
      ui->cbFirst->setChecked(true);
    }
    transmit (snr);
    ui->signal_meter_widget->setValue(0,0);
    if(m_mode=="Echo" and !m_tune) m_bTransmittedEcho=true;

    if((m_mode=="WSPR" or m_mode=="FST4W") and !m_tune) {
      if (m_config.TX_messages ()) {
        t = " Transmitting " + m_mode + " ----------------------- " +
          m_config.bands ()->find (m_freqNominal);
        t=beacon_start_time (m_TRperiod / 2) + ' ' + t.rightJustified (66, '-');
        ui->decodedTextBrowser->appendText(t);
      }
      write_all("Tx",m_currentMessage);
    }
  }
}

void MainWindow::stopTx()
{
  Q_EMIT endTransmitMessage ();
  m_btxok = false;
  m_transmitting = false;
  g_iptt=0;
  if (!m_tx_watchdog) {
    tx_status_label.setStyleSheet("");
    tx_status_label.setText("");
  }
  ptt0Timer.start(200);                       //end-of-transmission sequencer delay
  monitor (true);
  statusUpdate ();
}

void MainWindow::stopTx2()
{
  m_config.transceiver_ptt (false); //Lower PTT
  if (m_mode == "JT9" && m_bFast9
      && ui->cbAutoSeq->isVisible () && ui->cbAutoSeq->isEnabled () && ui->cbAutoSeq->isChecked ()
      && m_ntx == 5 && m_nTx73 >= 5) {
    on_stopTxButton_clicked ();
    m_nTx73 = 0;
  }
  if(((m_mode=="WSPR" or m_mode=="FST4W") and m_ntr==-1) and !m_tuneup) {
    m_wideGraph->setWSPRtransmitted();
    WSPR_scheduling ();
    m_ntr=0;
  }
  last_tx_label.setText(tr ("Last Tx: %1").arg (m_currentMessage.trimmed()));
}

void MainWindow::ba2msg(QByteArray ba, char message[])             //ba2msg()
{
  int iz=ba.length();
  for(int i=0; i<37; i++) {
    if(i<iz) {
      if(int(ba[i])>=97 and int(ba[i])<=122) ba[i]=int(ba[i])-32;
      message[i]=ba[i];
    } else {
      message[i]=32;
    }
  }
  message[37]=0;
}

void MainWindow::on_txFirstCheckBox_stateChanged(int nstate)        //TxFirst
{
  m_txFirst = (nstate==2);
}

void MainWindow::set_dateTimeQSO(int m_ntx)
{
    // m_ntx = -1 resets to default time
    // Our QSO start time can be fairly well determined from Tx 2 and Tx 3 -- the grid reports
    // If we CQ'd and sending sigrpt then 2 minutes ago n=2
    // If we're on msg 3 then 3 minutes ago n=3 -- might have sat on msg1 for a while
    // If we've already set our time on just return.
    // This should mean that Tx2 or Tx3 has been repeated so don't update the start time
    // We reset it in several places
    if (m_ntx == -1) { // we use a default date to detect change
      m_dateTimeQSOOn = QDateTime {};
    }
    else if (m_dateTimeQSOOn.isValid ()) {
        return;
    }
    else { // we also take of m_TRperiod/2 to allow for late clicks
      auto now = QDateTime::currentDateTimeUtc();
      m_dateTimeQSOOn = now.addSecs (-(m_ntx - 2) * int(m_TRperiod) -
                                     int(fmod(double(now.time().second()),m_TRperiod)));
    }
}

void MainWindow::set_ntx(int n)                                   //set_ntx()
{
  m_ntx=n;
}

void MainWindow::on_txrb1_toggled (bool status)
{
  if (status) {
    if (ui->tx1->isEnabled ()) {
      m_ntx = 1;
      set_dateTimeQSO (-1); // we reset here as tx2/tx3 is used for start times
    }
    else {
      QTimer::singleShot (0, ui->txrb2, SLOT (click ()));
    }
  }
}

bool MainWindow::elide_tx1_not_allowed () const
{
  auto const& my_callsign = m_config.my_callsign ();
  return
    (m_mode=="FT8" && SpecOp::HOUND == m_config.special_op_id())
    || ((m_mode.startsWith ("FT") || "MSK144" == m_mode || "Q65" == m_mode || "FST4" == m_mode)
        && Radio::is_77bit_nonstandard_callsign (my_callsign))
    || (my_callsign != m_baseCall && !shortList (my_callsign));
}

void MainWindow::on_txrb1_doubleClicked ()
{
  ui->tx1->setEnabled (elide_tx1_not_allowed () || !ui->tx1->isEnabled ());
  if (!ui->tx1->isEnabled ()) {
    // leave time for clicks to complete before setting txrb2
    QTimer::singleShot (500, ui->txrb2, SLOT (click ()));
  }
}

void MainWindow::on_txrb2_toggled (bool status)
{
  // Tx 2 means we already have CQ'd so good reference
  if (status) {
    m_ntx=2;
    set_dateTimeQSO (m_ntx);
  }
}

void MainWindow::on_txrb3_toggled(bool status)
{
  // Tx 3 means we should have already have done Tx 1 so good reference
  if (status) {
    m_ntx=3;
    set_dateTimeQSO(m_ntx);
  }
}

void MainWindow::on_txrb4_toggled (bool status)
{
  if (status) {
    m_ntx=4;
  }
}

void MainWindow::on_txrb4_doubleClicked ()
{
  // RR73 only allowed if not a type 2 compound callsign
  auto const& my_callsign = m_config.my_callsign ();
  auto is_compound = my_callsign != m_baseCall;
  m_send_RR73 = !((is_compound && !shortList (my_callsign)) || m_send_RR73);
  if(m_mode=="FT4") m_send_RR73=true;
  genStdMsgs (m_rpt);
}

void MainWindow::on_txrb5_toggled (bool status)
{
  if (status) {
    m_ntx = 5;
  }
}

void MainWindow::on_txrb5_doubleClicked ()
{
  genStdMsgs (m_rpt, true);
}

void MainWindow::on_txrb6_toggled(bool status)
{
  if (status) {
    m_ntx=6;
    if (ui->txrb6->text().contains (QRegularExpression {"^(CQ|QRZ) "})) set_dateTimeQSO(-1);
  }
}

void MainWindow::on_txb1_clicked()
{
  if (ui->tx1->isEnabled ()) {
    m_ntx=1;
    m_QSOProgress = REPLYING;
    ui->txrb1->setChecked(true);
    if(m_transmitting) m_restart=true;
  }
  else {
    on_txb2_clicked ();
  }
}

void MainWindow::on_txb1_doubleClicked()
{
  ui->tx1->setEnabled (elide_tx1_not_allowed () || !ui->tx1->isEnabled ());
}

void MainWindow::on_txb2_clicked()
{
    m_ntx=2;
    m_QSOProgress = REPORT;
    ui->txrb2->setChecked(true);
    if(m_transmitting) m_restart=true;
}

void MainWindow::on_txb3_clicked()
{
    m_ntx=3;
    m_QSOProgress = ROGER_REPORT;
    ui->txrb3->setChecked(true);
    if(m_transmitting) m_restart=true;
}

void MainWindow::on_txb4_clicked()
{
    m_ntx=4;
    m_QSOProgress = ROGERS;
    ui->txrb4->setChecked(true);
    if(m_transmitting) m_restart=true;
}

void MainWindow::on_txb4_doubleClicked()
{
  // RR73 only allowed if not a type 2 compound callsign
  auto const& my_callsign = m_config.my_callsign ();
  auto is_compound = my_callsign != m_baseCall;
  m_send_RR73 = !((is_compound && !shortList (my_callsign)) || m_send_RR73);
  if(m_mode=="FT4") m_send_RR73=true;
  genStdMsgs (m_rpt);
}

void MainWindow::on_txb5_clicked()
{
    m_ntx=5;
    m_QSOProgress = SIGNOFF;
    ui->txrb5->setChecked(true);
    if(m_transmitting) m_restart=true;
}

void MainWindow::on_txb5_doubleClicked()
{
  genStdMsgs (m_rpt, true);
}

void MainWindow::on_txb6_clicked()
{
    m_ntx=6;
    m_QSOProgress = CALLING;
    set_dateTimeQSO(-1);
    ui->txrb6->setChecked(true);
    if(m_transmitting) m_restart=true;
}

void MainWindow::doubleClickOnCall2(Qt::KeyboardModifiers modifiers)
{
//Confusing: come here after double-click on left text window, not right window.
  set_dateTimeQSO(-1); // reset our QSO start time
  m_decodedText2=true;
  doubleClickOnCall(modifiers);
  m_decodedText2=false;
}

void MainWindow::doubleClickOnCall(Qt::KeyboardModifiers modifiers)
{
  QTextCursor cursor;
  if(m_mode=="FST4W") {
    MessageBox::information_message (this,
        "Double-click not available for FST4W mode");
    return;
  }
  if(m_decodedText2) {
    cursor=ui->decodedTextBrowser->textCursor();
  } else {
    cursor=ui->decodedTextBrowser2->textCursor();
  }

  if(modifiers==(Qt::ShiftModifier + Qt::ControlModifier + Qt::AltModifier)) {
    //### What was the purpose of this ???  ###
    cursor.setPosition(0);
  } else {
    cursor.setPosition(cursor.selectionStart());
  }

  if(SpecOp::FOX==m_config.special_op_id() and m_decodedText2) {
    if(m_houndQueue.count()<10 and m_nSortedHounds>0) {
      QString t=cursor.block().text();
      selectHound(t);
    }
    return;
  }
  DecodedText message {cursor.block().text().trimmed().left(61).remove("TU; ")};
  m_bDoubleClicked = true;
  processMessage (message, modifiers);
}

void MainWindow::processMessage (DecodedText const& message, Qt::KeyboardModifiers modifiers)
{
  // decode keyboard modifiers we are interested in
  auto shift = modifiers.testFlag (Qt::ShiftModifier);
  auto ctrl = modifiers.testFlag (Qt::ControlModifier);
  // auto alt = modifiers.testFlag (Qt::AltModifier);
  auto auto_seq = ui->cbAutoSeq->isVisible () && ui->cbAutoSeq->isEnabled () && ui->cbAutoSeq->isChecked ();
  // basic mode sanity checks
  auto const& parts = message.clean_string ().split (' ', SkipEmptyParts);
  if (parts.size () < 5) return;
  auto const& mode = parts.at (4).left (1);
  if (("JT65" == m_mode && mode != "#")
      || ("JT9" == m_mode && mode != "@")
      || ("MSK144" == m_mode && !("&" == mode || "^" == mode))
      || ("Q65" == m_mode && mode.left (1) != ":")) {
    return;      //Currently we do auto-sequencing only in FT4, FT8, MSK144, FST4, and Q65
  }

  //Skip the rest if no decoded text extracted
  int frequency = message.frequencyOffset();
  if (message.isTX()) {
    if (!m_config.enable_VHF_features()) {
      if(!shift) ui->RxFreqSpinBox->setValue(frequency); //Set Rx freq
      if((ctrl or shift) and !ui->cbHoldTxFreq->isChecked ()) {
        ui->TxFreqSpinBox->setValue(frequency); //Set Tx freq
      }
    }
    return;
  }

  // check for CQ with listening frequency
  if (parts.size () >= 7
      && (m_bFastMode || m_mode=="FT8")
      && "CQ" == parts[5]
      && m_config.is_transceiver_online ()) {
    bool ok;
    auto kHz = parts[6].toUInt (&ok);
    if (ok && kHz >= 10 && 3 == parts[6].size ()) {
      // QSY Freq for answering CQ nnn
      setRig (m_freqNominal / 1000000 * 1000000 + 1000 * kHz);
      ui->decodedTextBrowser2->displayQSY (QString {"QSY %1"}.arg (m_freqNominal / 1e6, 7, 'f', 3));
    }
  }

  int nmod = fmod(double(message.timeInSeconds()),2.0*m_TRperiod);
  m_txFirst=(nmod!=0);
  if( SpecOp::HOUND == m_config.special_op_id() ) m_txFirst=false;          //Hound must not transmit first
  if( SpecOp::FOX == m_config.special_op_id() ) m_txFirst=true;             //Fox must always transmit first
  ui->txFirstCheckBox->setChecked(m_txFirst);

  auto const& message_words = message.messageWords ();
  if (message_words.size () < 3) return;

  QString hiscall;
  QString hisgrid;
  message.deCallAndGrid(/*out*/hiscall,hisgrid);


  if(message.clean_string ().contains(hiscall+"/R")) {
    hiscall+="/R";
    ui->dxCallEntry->setText(hiscall);
  }
  if(message.clean_string ().contains(hiscall+"/P")) {
    hiscall+="/P";
    ui->dxCallEntry->setText(hiscall);
  }

  //SP6XD I found this place as the best for extracting remote callsign
  //and decide wheter to answer or not
  if ( ui->ignoreRegex->isChecked() && !m_config.regex_filter().isEmpty() && hiscall.contains(QRegularExpression(m_config.regex_filter())) ) {
    ui->lastIgnored->setStyleSheet("QLabel {background-color: red; color: white;}");
    ui->lastIgnored->setText(hiscall);
    qDebug () << "Ignoring station:" << hiscall << " grid:" << hisgrid;
    //printf("Ignoring %s\n ", hiscall.toStdString().c_str());
    return;
  } else {
    qDebug () << "Processing station:" << hiscall << " grid:" << hisgrid;
    //printf("Processing %s\n ", hiscall.toStdString().c_str());
  }

  QStringList w=message.clean_string ().mid(22).remove("<").remove(">").split(" ",SkipEmptyParts);
  int nw=w.size();
  if(nw>=4) {
    if(message_words.size()<4) return;
    int n=w.at(nw-2).toInt();
    if(n>=520001 and n<=592047) {
      hiscall=w.at(1);
      hisgrid=w.at(nw-1);
    }
  }

  bool is_73 = message_words.filter (QRegularExpression {"^(73|RR73)$"}).size ();
  if (!is_73 and !message.isStandardMessage() and !message.clean_string ().contains("<")) {
    qDebug () << "Not processing message - hiscall:" << hiscall << "hisgrid:" << hisgrid
              << message.clean_string () << message.isStandardMessage();
    return;
  }

  if ((message.isJT9 () and m_mode != "JT9" and m_mode != "JT4") or
             (message.isJT65 () and m_mode != "JT65" and m_mode != "JT4")) {
    // We are not allowing mode change, so don't process decode
    return;
  }

  // ignore calls by other hounds
  if (SpecOp::HOUND == m_config.special_op_id()
      && message.messageWords ().indexOf (QRegularExpression {R"(R\+-[0-9]+)"}) >= 1)
    {
      return;
    }

  QString firstcall = message.call();
  if(firstcall.length()==5 and firstcall.mid(0,3)=="CQ ") firstcall="CQ";
  if(!m_bFastMode and (!m_config.enable_VHF_features() or m_mode=="FT8")) {
    // Don't change Tx freq if in a fast mode, or VHF features enabled; also not if a
    // station is calling me, unless CTRL or SHIFT is held down.
    if ((Radio::is_callsign (firstcall)
         && firstcall != m_config.my_callsign () && firstcall != m_baseCall
         && firstcall != "DE")
        || "CQ" == firstcall || "QRZ" == firstcall || ctrl || shift) {
      if (((SpecOp::HOUND != m_config.special_op_id()) || m_mode != "FT8")
          && (!ui->cbHoldTxFreq->isChecked () || shift || ctrl)) {
        ui->TxFreqSpinBox->setValue(frequency);
      }
      if(m_mode != "JT4" && m_mode != "JT65" && !m_mode.startsWith ("JT9") &&
         m_mode != "Q65" && m_mode!="FT8" && m_mode!="FT4" && m_mode!="FST4") {
        return;
      }
    }
  }

  // prior DX call (possible QSO partner)
  auto qso_partner_base_call = Radio::base_callsign (ui->dxCallEntry->text ());
  auto base_call = Radio::base_callsign (hiscall);

// Determine appropriate response to received message
  auto dtext = " " + message.clean_string () + " ";
  dtext=dtext.remove("<").remove(">");
  if(dtext.contains (" " + m_baseCall + " ")
     || dtext.contains ("<" + m_baseCall + "> ")
//###???     || dtext.contains ("<" + m_baseCall + " " + hiscall + "> ")
     || dtext.contains ("/" + m_baseCall + " ")
     || dtext.contains (" " + m_baseCall + "/")
     || (firstcall == "DE")) {

    QString w2;
    int nw=w.size();
    if(nw>=3) w2=w.at(2);
    int nrpt=w2.toInt();
    QString w34;
    if(nw>=4) {
//      w34=w.at(nw-2);
      nrpt=w.at(nw-2).toInt();
      w34=w.at(nw-1);
    }
    bool bRTTY = (nrpt>=529 and nrpt<=599);
    bool bEU_VHF_w2=(nrpt>=520001 and nrpt<=594000);
    if(bEU_VHF_w2 and SpecOp::EU_VHF!=m_config.special_op_id()) {
      auto const& msg = tr("Should you switch to EU VHF Contest mode?\n\n"
                               "To do so, check 'Special operating activity' and\n"
                               "'EU VHF Contest' on the Settings | Advanced tab.");
      MessageBox::information_message (this, msg);
    }

    QStringList t=message.clean_string ().split(' ', SkipEmptyParts);
    int n=t.size();
    QString t0=t.at(n-2);
    QString t1=t0.right(1);
    bool bFieldDay_msg = (t1>="A" and t1<="F" and t0.size()<=3 and n>=9);
    int m=t0.remove(t1).toInt();
    if(m < 1) bFieldDay_msg=false;
    if(bFieldDay_msg) {
      m_xRcvd=t.at(n-2) + " " + t.at(n-1);
      t0=t.at(n-3);
    }
    if(bFieldDay_msg and SpecOp::FIELD_DAY!=m_config.special_op_id()) {
      // ### Should be in ARRL Field Day mode ??? ###
      MessageBox::information_message (this, tr ("Should you switch to ARRL Field Day mode?"));
    }

    if(bRTTY and SpecOp::RTTY != m_config.special_op_id()) {
      // ### Should be in RTTY contest mode ??? ###
      MessageBox::information_message (this, tr ("Should you switch to RTTY contest mode?"));
    }

    if(SpecOp::EU_VHF==m_config.special_op_id() and message_words.at(2).contains(m_baseCall) and
       (!message_words.at(3).contains(qso_partner_base_call)) and (!m_bDoubleClicked)) {
      return;
    }

    bool bContestOK=(m_mode=="FT4" or m_mode=="FT8" or m_mode=="Q65" or m_mode=="MSK144");
    if(message_words.size () > 4   // enough fields for a normal message
       && (message_words.at(2).contains(m_baseCall) || "DE" == message_words.at(2))
       && (message_words.at(3).contains(qso_partner_base_call) or m_bDoubleClicked
           or bEU_VHF_w2 or (m_QSOProgress==CALLING))) {
      if(message_words.at(4).contains(grid_regexp) and SpecOp::EU_VHF!=m_config.special_op_id()) {
        if((SpecOp::NA_VHF==m_config.special_op_id() or SpecOp::WW_DIGI==m_config.special_op_id()) and bContestOK){
          setTxMsg(3);
          m_QSOProgress=ROGER_REPORT;
        } else {
          if(m_mode=="JT65" and message_words.size()>5 and message_words.at(5)=="OOO") {
            setTxMsg(3);
            m_QSOProgress=ROGER_REPORT;
          } else {
            setTxMsg(2);
            m_QSOProgress=REPORT;
          }
        }
      } else if(w34.contains(grid_regexp) and SpecOp::EU_VHF==m_config.special_op_id()) {

        if(nrpt==0) {
          setTxMsg(2);
          m_QSOProgress=REPORT;
        } else {
          if(w2=="R") {
            setTxMsg(4);
            m_QSOProgress=ROGERS;
          } else {
            setTxMsg(3);
            m_QSOProgress=ROGER_REPORT;
          }
        }
      } else if(SpecOp::RTTY == m_config.special_op_id() and bRTTY) {
        if(w2=="R") {
          setTxMsg(4);
          m_QSOProgress=ROGERS;
        } else {
          setTxMsg(3);
          m_QSOProgress=ROGER_REPORT;
        }
        m_xRcvd=t[n-2] + " " + t[n-1];
      } else if(SpecOp::FIELD_DAY==m_config.special_op_id() and bFieldDay_msg) {
        if(t0=="R") {
          setTxMsg(4);
          m_QSOProgress=ROGERS;
        } else {
          setTxMsg(3);
          m_QSOProgress=ROGER_REPORT;
        }
      } else {  // no grid on end of msg
        auto const& word_3 = message_words.at (4);
        auto word_3_as_number = word_3.toInt ();
        if (("RRR" == word_3
             || (word_3_as_number == 73 && ROGERS == m_QSOProgress)
             || "RR73" == word_3
             || ("R" == word_3 && m_QSOProgress != REPORT))) {
          if(m_mode=="FT4" and "RR73" == word_3) m_dateTimeRcvdRR73=QDateTime::currentDateTimeUtc();
          m_bTUmsg=false;
          m_nextCall="";   //### Temporary: disable use of "TU;" message
          if(SpecOp::RTTY == m_config.special_op_id() and m_nextCall!="") {
            // We're in RTTY contest and have "nextCall" queued up: send a "TU; ..." message
            if (m_config.prompt_to_log() || m_config.autoLog()) {
              logQSOTimer.start(0);
            }
            else {
              cease_auto_Tx_after_QSO ();
            }
            ui->tx3->setText(ui->tx3->text().remove("TU; "));
            useNextCall();
            QString t="TU; " + ui->tx3->text();
            ui->tx3->setText(t);
            m_bTUmsg=true;
          } else {
            if (m_QSOProgress > CALLING && m_QSOProgress < SIGNOFF
                && SpecOp::NONE < m_config.special_op_id () && SpecOp::FOX > m_config.special_op_id ()
                && ("RR73" == word_3 || 73 == word_3_as_number))
              {
                if (m_config.prompt_to_log() || m_config.autoLog()) {
                  logQSOTimer.start(0);
                }
                else {
                  cease_auto_Tx_after_QSO ();
                }
                m_ntx=6;
                ui->txrb6->setChecked(true);
              }
            else if (word_3.contains (QRegularExpression {"^R(?!R73|RR)"})
                     && m_QSOProgress != ROGER_REPORT)
              {
                m_ntx=4;
                ui->txrb4->setChecked(true);
              }
            else if ((m_QSOProgress > CALLING && m_QSOProgress < ROGERS)
                     || word_3.contains (QRegularExpression {"^RR(?:R|73)$"}))
              {
                m_ntx=5;
                ui->txrb5->setChecked(true);
              }
            else if (ROGERS == m_QSOProgress)
              {
                if (m_config.prompt_to_log() || m_config.autoLog()) {
                  logQSOTimer.start(0);
                }
                else {
                  cease_auto_Tx_after_QSO ();
                }
                m_ntx=6;
                ui->txrb6->setChecked(true);
              }
            else
              {
                // just work them (again)
                if (ui->tx1->isEnabled ()) {
                  m_ntx = 1;
                  m_QSOProgress = REPLYING;
                  ui->txrb1->setChecked (true);
                } else {
                  m_ntx=2;
                  m_QSOProgress = REPORT;
                  ui->txrb2->setChecked (true);
                }
              }
          }
          if (m_QSOProgress >= ROGER_REPORT)
            {
              m_QSOProgress = SIGNOFF;
            }
        } else if((m_QSOProgress >= REPORT
                   || (m_QSOProgress >= REPLYING &&
                   (m_mode=="MSK144" or m_mode=="FT8" or m_mode=="FT4" || "Q65" == m_mode)))
                  && word_3.startsWith ('R')) {
          m_ntx=4;
          m_QSOProgress = ROGERS;
          if(SpecOp::RTTY == m_config.special_op_id()) {
            int n=t.size();
            int nRpt=t[n-2].toInt();
            if(nRpt>=529 and nRpt<=599) m_xRcvd=t[n-2] + " " + t[n-1];
          }
          ui->txrb4->setChecked(true);
        } else if (m_QSOProgress >= CALLING)
          {
            if ((word_3_as_number >= -50 && word_3_as_number <= 49)
                || (word_3_as_number >= 529 && word_3_as_number <= 599))
              {
                if(SpecOp::EU_VHF==m_config.special_op_id() or
                   SpecOp::FIELD_DAY==m_config.special_op_id() or
                   SpecOp::RTTY==m_config.special_op_id())
                  {
                    setTxMsg(2);
                    m_QSOProgress=REPORT;
                  }
                else
                  {
                    if (word_3.startsWith ("R-") || word_3.startsWith ("R+"))
                      {
                        setTxMsg(4);
                        m_QSOProgress=ROGERS;
                      }
                    else
                      {
                        setTxMsg (3);
                        m_QSOProgress = ROGER_REPORT;
                      }
                  }
              }
          }
        else
          {                // nothing for us
            return;
          }
      }
    }
    else if (5 == message_words.size ()
             && m_baseCall == message_words.at (1)) {
      // dual Fox style message, possibly from MSHV
      if (m_config.prompt_to_log() || m_config.autoLog()) {
        logQSOTimer.start(0);
      }
      else {
        cease_auto_Tx_after_QSO ();
      }
      m_ntx=6;
      ui->txrb6->setChecked(true);
    }
    else if (m_QSOProgress >= ROGERS
             && message_words.size () > 3 && message_words.at (2).contains (m_baseCall)
             && message_words.at (3) == "73") {
      // 73 back to compound call holder
      m_ntx=5;
      ui->txrb5->setChecked(true);
      m_QSOProgress = SIGNOFF;
    }
    else if (!(m_bAutoReply && (m_QSOProgress > CALLING))) {
      if ((message_words.size () > 5 && message_words.at (2).contains (m_baseCall)
           && message_words.at (5) == "OOO")) {
        // EME short code report or MSK144/FT8 contest mode reply, send back Tx3
        m_ntx=3;
        m_QSOProgress = ROGER_REPORT;
        ui->txrb3->setChecked (true);
      } else if (!is_73) {    // don't respond to sign off messages
        m_ntx=2;
        m_QSOProgress = REPORT;
        ui->txrb2->setChecked(true);
        if (m_bDoubleClickAfterCQnnn and m_transmitting) {
          on_stopTxButton_clicked();
          TxAgainTimer.start(1500);
        }
        m_bDoubleClickAfterCQnnn=false;
      }
      else {
        return;               // nothing we need to respond to
      }
    }
    else {                  // nothing for us
      return;
    }
  }
  else if (firstcall == "DE" && message_words.size () > 4 && message_words.at (4) == "73") {
    if (m_QSOProgress >= ROGERS && base_call == qso_partner_base_call && m_currentMessageType) {
      // 73 back to compound call holder
      m_ntx=5;
      ui->txrb5->setChecked(true);
      m_QSOProgress = SIGNOFF;
    } else {
      // treat like a CQ/QRZ
      if (ui->tx1->isEnabled ()) {
        m_ntx = 1;
        m_QSOProgress = REPLYING;
        ui->txrb1->setChecked (true);
      } else {
        m_ntx=2;
        m_QSOProgress = REPORT;
        ui->txrb2->setChecked (true);
      }
    }
  }
  else if (is_73 && !message.isStandardMessage ()) {
    m_ntx=5;
    ui->txrb5->setChecked(true);
    m_QSOProgress = SIGNOFF;
  } else {
    // just work them
    if (ui->tx1->isEnabled ()) {
      m_ntx = 1;
      m_QSOProgress = REPLYING;
      ui->txrb1->setChecked (true);
    } else {
      m_ntx=2;
      m_QSOProgress = REPORT;
      ui->txrb2->setChecked (true);
    }
  }
  // if we get here then we are reacting to the message
  if (m_bAutoReply) m_bCallingCQ = CALLING == m_QSOProgress;
  if (ui->RxFreqSpinBox->isEnabled () and m_mode != "MSK144" and !shift) {
    ui->RxFreqSpinBox->setValue (frequency);    //Set Rx freq
  }

  QString s1 = m_QSOText.trimmed ();
  QString s2 = message.clean_string ().trimmed();
  if (s1!=s2 and !message.isTX()) {
    if (!s2.contains(m_baseCall) or m_mode=="MSK144") {  // Taken care of elsewhere if for_us and slow mode
      ui->decodedTextBrowser2->displayDecodedText (message, m_config.my_callsign (), m_mode, m_config.DXCC (),
      m_logBook, m_currentBand, m_config.ppfx ());
    }
    m_QSOText = s2;
  }

  if (Radio::is_callsign (hiscall)
      && (base_call != qso_partner_base_call || base_call != hiscall)) {
    if (qso_partner_base_call != base_call) {
      // clear the DX grid if the base call of his call is different
      // from the current DX call
      ui->dxGridEntry->clear ();
    }
    // his base call different or his call more qualified
    // i.e. compound version of same base call
    ui->dxCallEntry->setText (hiscall);
  }
  if (hisgrid.contains (grid_regexp)) {
    if(ui->dxGridEntry->text().mid(0,4) != hisgrid) ui->dxGridEntry->setText(hisgrid);
  }
  lookup();
  m_hisGrid = ui->dxGridEntry->text();

  if (m_bDoubleClicked)
    {
      // extract our report if present
      message.report (m_baseCall, Radio::base_callsign(ui->dxCallEntry->text()), m_rptRcvd);
    }

  if (!m_bSentReport || base_call != qso_partner_base_call) // Don't change report within a QSO
    {
      auto n = message.report ().toInt ();
      if(m_mode=="MSK144" and m_bShMsgs) {
        if(n<=-2) n=-3;
        if(n>=-1 and n<=1) n=0;
        if(n>=2 and n<=4) n=3;
        if(n>=5 and n<=7) n=6;
        if(n>=8 and n<=11) n=10;
        if(n>=12 and n<=14) n=13;
        if(n>=15) n=16;
      }
      ui->rptSpinBox->setValue (n);
    }
// Don't genStdMsgs if we're already sending 73, or a "TU; " msg is queued.
  m_bTUmsg=false;   //### Temporary: disable use of "TU;" messages
  if (!m_nTx73 and !m_bTUmsg) {
    genStdMsgs (QString::number (ui->rptSpinBox->value ()));
  }
  if(m_transmitting) m_restart=true;
  if (auto_seq && !m_bDoubleClicked && m_mode!="FT4") {
    return;
  }
  if(m_config.quick_call() && m_bDoubleClicked) auto_tx_mode(true);
  m_bDoubleClicked=false;
}

void MainWindow::setTxMsg(int n)
{
  m_ntx=n;
  if(n==1) ui->txrb1->setChecked(true);
  if(n==2) ui->txrb2->setChecked(true);
  if(n==3) ui->txrb3->setChecked(true);
  if(n==4) ui->txrb4->setChecked(true);
  if(n==5) ui->txrb5->setChecked(true);
  if(n==6) ui->txrb6->setChecked(true);
}

void MainWindow::genCQMsg ()
{
  auto const& my_callsign = m_config.my_callsign ();
  auto is_compound = my_callsign != m_baseCall;
  auto is_type_two = !is77BitMode () && is_compound && stdCall (m_baseCall) && !shortList (my_callsign);
  if(my_callsign.size () && m_config.my_grid().size ()) {
    auto const& grid = m_config.my_grid ();
    if (ui->cbCQTx->isEnabled () && ui->cbCQTx->isVisible () && ui->cbCQTx->isChecked ()) {
      if(stdCall (my_callsign)
         || is_type_two) {
        msgtype (QString {"CQ %1 %2 %3"}
               .arg (m_freqNominal / 1000 - m_freqNominal / 1000000 * 1000, 3, 10, QChar {'0'})
               .arg (my_callsign)
               .arg (grid.left (4)),
               ui->tx6);
      } else {
        msgtype (QString {"CQ %1 %2"}
               .arg (m_freqNominal / 1000 - m_freqNominal / 1000000 * 1000, 3, 10, QChar {'0'})
               .arg (my_callsign),
               ui->tx6);
      }
    } else {
      if (stdCall (my_callsign)
          || is_type_two) {
        msgtype (QString {"%1 %2 %3"}.arg(m_CQtype).arg(my_callsign)
                 .arg(grid.left(4)),ui->tx6);
      } else {
        msgtype (QString {"%1 %2"}.arg(m_CQtype).arg(my_callsign),ui->tx6);
      }
    }
    if ((m_mode=="JT4" or m_mode=="Q65") and  ui->cbShMsgs->isChecked()) {
      if (ui->cbTx6->isChecked ()) {
        msgtype ("@1250  (SEND MSGS)", ui->tx6);
      } else {
        msgtype ("@1000  (TUNE)", ui->tx6);
      }
    }

    QString t=ui->tx6->text();
    QStringList tlist=t.split(" ");
    if((m_mode=="FT4" or m_mode=="FT8" or m_mode=="MSK144" || "Q65" == m_mode) and
       SpecOp::NONE != m_config.special_op_id() and
       ( tlist.at(1)==my_callsign or
         tlist.at(2)==my_callsign ) and
       stdCall(my_callsign)) {
      if(SpecOp::NA_VHF == m_config.special_op_id())    m_cqStr="TEST";
      if(SpecOp::EU_VHF == m_config.special_op_id())    m_cqStr="TEST";
      if(SpecOp::FIELD_DAY == m_config.special_op_id()) m_cqStr="FD";
      if(SpecOp::RTTY == m_config.special_op_id())      m_cqStr="RU";
      if(SpecOp::WW_DIGI == m_config.special_op_id())   m_cqStr="WW";
      if( tlist.at(1)==my_callsign ) {
         t="CQ " + m_cqStr + " " + tlist.at(1) + " " + tlist.at(2); 
      } else {
         t="CQ " + m_cqStr + " " + tlist.at(2) + " " + tlist.at(3);
      }
      ui->tx6->setText(t);
    }
  } else {
    ui->tx6->clear ();
  }
}

void MainWindow::abortQSO()
{
  bool b=m_auto;
  clearDX();
  if(b) auto_tx_mode(false);
  ui->txrb6->setChecked(true);
}

bool MainWindow::stdCall(QString const& w)
{
  static QRegularExpression standard_call_re {
    R"(
        ^\s*                                      # optional leading spaces
        ( [A-Z]{0,2} | [A-Z][0-9] | [0-9][A-Z] )  # part 1
        ( [0-9][A-Z]{0,3} )                       # part 2
        (/R | /P)?                                # optional suffix
        \s*$                                      # optional trailing spaces
    )", QRegularExpression::CaseInsensitiveOption | QRegularExpression::ExtendedPatternSyntaxOption};
  return standard_call_re.match (w).hasMatch ();
}

bool MainWindow::is77BitMode () const
{
  return "FT8" == m_mode || "FT4" == m_mode || "MSK144" == m_mode
    || "FST4" == m_mode || "Q65" == m_mode;
}

void MainWindow::genStdMsgs(QString rpt, bool unconditional)
{
  genCQMsg ();
  auto const& hisCall=ui->dxCallEntry->text();
  if(!hisCall.size ()) {
    ui->labAz->clear ();
    ui->tx1->clear ();
    ui->tx2->clear ();
    ui->tx3->clear ();
    ui->tx4->clear ();
    if(unconditional) ui->tx5->lineEdit ()->clear ();   //Test if it needs sending again
    m_gen_message_is_cq = false;
    return;
  }
  auto const& my_callsign = m_config.my_callsign ();
  auto is_compound = my_callsign != m_baseCall;
  auto is_type_one = !is77BitMode () && is_compound && shortList (my_callsign);
  auto const& my_grid = m_config.my_grid ().left (4);
  auto const& hisBase = Radio::base_callsign (hisCall);
  save_dxbase_(const_cast <char *> ((hisBase + "   ").left (6).toLatin1().constData()),6);
  auto eme_short_codes = m_config.enable_VHF_features () && ui->cbShMsgs->isChecked ()
      && m_mode == "JT65";

  bool bMyCall=stdCall(my_callsign);
  bool bHisCall=stdCall(hisCall);

  QString t0=hisBase + " " + m_baseCall + " ";
  QString t0s=hisCall + " " + my_callsign + " ";
  QString t0a,t0b;

  if (is77BitMode () && bHisCall && bMyCall) t0=hisCall + " " + my_callsign + " ";
  t0a="<"+hisCall + "> " + my_callsign + " ";
  t0b=hisCall + " <" + my_callsign + "> ";

  QString t00=t0;
  QString t {t0 + my_grid};
  if(!bMyCall) t=t0a;
  msgtype(t, ui->tx1);
  if (eme_short_codes) {
    t=t+" OOO";
    if(!bHisCall) t=hisCall + " " + m_baseCall + " OOO";
    if(!bMyCall) t=hisBase + " " + my_callsign + " OOO";
    msgtype(t, ui->tx2);
    msgtype("RO", ui->tx3);
    msgtype("RRR", ui->tx4);
    msgtype("73", ui->tx5->lineEdit());
  } else {
    int n=rpt.toInt();
    rpt = rpt.asprintf("%+2.2d",n);

    if (is77BitMode ()) {
      QString t2,t3;
      QString sent=rpt;
      QString rs,rst;
      int nn=(n+36)/6;
      if(nn<2) nn=2;
      if(nn>9) nn=9;
      rst = rst.asprintf("5%1d9 ",nn);
      rs=rst.mid(0,2);
      t=t0;
      if(!bMyCall) {
        t=t0b;
        msgtype(t0a, ui->tx1);
      }
      if(!bHisCall) {
        t=t0a;
        msgtype(t0a + my_grid, ui->tx1);
      }
      if(SpecOp::NA_VHF==m_config.special_op_id()) sent=my_grid;
      if(SpecOp::WW_DIGI==m_config.special_op_id()) sent=my_grid;
      if(SpecOp::FIELD_DAY==m_config.special_op_id()) sent=m_config.Field_Day_Exchange();
      if(SpecOp::RTTY==m_config.special_op_id()) { 
        sent=rst + m_config.RTTY_Exchange();
        QString t1=m_config.RTTY_Exchange();
        if(t1=="DX" or t1=="#") {
          t1 = t1.asprintf("%4.4d",ui->sbSerialNumber->value());
          sent=rst + t1;
        }
      }
      if(SpecOp::EU_VHF==m_config.special_op_id()) {
        QString a;
        t="<" + t0s.split(" ").at(0) + "> <" + t0s.split(" ").at(1) + "> ";
        a = a.asprintf("%4.4d ",ui->sbSerialNumber->value());
        sent=rs + a + m_config.my_grid();
      }
      msgtype(t + sent, ui->tx2);
      if(sent==rpt) msgtype(t + "R" + sent, ui->tx3);
      if(sent!=rpt) msgtype(t + "R " + sent, ui->tx3);
      if(m_mode=="FT4" and SpecOp::RTTY==m_config.special_op_id()) {
        QDateTime now=QDateTime::currentDateTimeUtc();
        int sinceTx3 = m_dateTimeSentTx3.secsTo(now);
        int sinceRR73 = m_dateTimeRcvdRR73.secsTo(now);
        if(m_bDoubleClicked and (sinceTx3 < 15) and (sinceRR73 < 3)) {
          t="TU; " + ui->tx3->text();
          ui->tx3->setText(t);
        }
      }
    }

    if(m_mode=="MSK144" and m_bShMsgs) {
      int i=t0s.length()-1;
      t0="<" + t0s.mid(0,i) + "> ";
      if(SpecOp::NA_VHF != m_config.special_op_id()) {
        if(n<=-2) n=-3;
        if(n>=-1 and n<=1) n=0;
        if(n>=2 and n<=4) n=3;
        if(n>=5 and n<=7) n=6;
        if(n>=8 and n<=11) n=10;
        if(n>=12 and n<=14) n=13;
        if(n>=15) n=16;
        rpt = rpt.asprintf("%+2.2d",n);
      }
    }

    if (!is77BitMode ()) {
      t=(is_type_one ? t0 : t00) + rpt;
      msgtype(t, ui->tx2);
      t=t0 + "R" + rpt;
      msgtype(t, ui->tx3);
    }

    if(m_mode=="MSK144" and m_bShMsgs) {
      if(m_config.special_op_id()==SpecOp::NONE) {
        t=t0 + "R" + rpt;
        msgtype(t, ui->tx3);
      }
      m_send_RR73=false;
    }

    t=t0 + (m_send_RR73 ? "RR73" : "RRR");
    if((m_mode=="MSK144" and !m_bShMsgs) or m_mode=="FT8" or m_mode=="FT4" || m_mode == "FST4" || m_mode == "Q65") {
      if(!bHisCall and bMyCall) t=hisCall + " <" + my_callsign + "> " + (m_send_RR73 ? "RR73" : "RRR");
      if(bHisCall and !bMyCall) t="<" + hisCall + "> " + my_callsign + " " + (m_send_RR73 ? "RR73" : "RRR");
    }
    if ((m_mode=="JT4" || m_mode=="Q65") && m_bShMsgs) t="@1500  (RRR)";
    msgtype(t, ui->tx4);

    t=t0 + "73";
    if((m_mode=="MSK144" and !m_bShMsgs) or m_mode=="FT8" or m_mode=="FT4" || m_mode == "FST4" || m_mode == "Q65") {
      if(!bHisCall and bMyCall) t=hisCall + " <" + my_callsign + "> 73";
      if(bHisCall and !bMyCall) t="<" + hisCall + "> " + my_callsign + " 73";
    }
    if (m_mode=="JT4" || m_mode=="Q65") {
      if (m_bShMsgs) t="@1750  (73)";
      msgtype(t, ui->tx5->lineEdit());
    } else if ("MSK144" == m_mode && m_bShMsgs) {
      msgtype(t, ui->tx5->lineEdit());
    } else if(unconditional || hisBase != m_lastCallsign || !m_lastCallsign.size ()) {
      // only update tx5 when forced or  callsign changes
      msgtype(t, ui->tx5->lineEdit());
      m_lastCallsign = hisBase;
    }
  }

  if (is77BitMode ()) return;

  if (is_compound) {
    if (is_type_one) {
      t=hisBase + " " + my_callsign;
      msgtype(t, ui->tx1);
    } else {
      t = "DE " + my_callsign + " ";
      switch (m_config.type_2_msg_gen ())
        {
        case Configuration::type_2_msg_1_full:
          msgtype(t + my_grid, ui->tx1);
          if (!eme_short_codes) {
            if(is77BitMode () && SpecOp::NA_VHF == m_config.special_op_id()) {
              msgtype(t + "R " + my_grid, ui->tx3); // #### Unreachable code
            } else {
              msgtype(t + "R" + rpt, ui->tx3);
            }
            if ((m_mode != "JT4" && m_mode != "Q65") || !m_bShMsgs) {
              msgtype(t + "73", ui->tx5->lineEdit ());
            }
          }
          break;

        case Configuration::type_2_msg_3_full:
          if (is77BitMode () && SpecOp::NA_VHF == m_config.special_op_id()) {
            msgtype(t + "R " + my_grid, ui->tx3);
            msgtype(t + "RRR", ui->tx4);
          } else {
            msgtype(t00 + my_grid, ui->tx1);
            msgtype(t + "R" + rpt, ui->tx3);
          }
          if (!eme_short_codes && ((m_mode != "JT4" && m_mode != "Q65") || !m_bShMsgs)) {
            msgtype(t + "73", ui->tx5->lineEdit ());
          }
          break;

        case Configuration::type_2_msg_5_only:
          msgtype(t00 + my_grid, ui->tx1);
          if (!eme_short_codes) {
            if (is77BitMode () && SpecOp::NA_VHF == m_config.special_op_id()) {
              msgtype(t + "R " + my_grid, ui->tx3); // #### Unreachable code
              msgtype(t + "RRR", ui->tx4);
            } else {
              msgtype(t0 + "R" + rpt, ui->tx3);
            }
          }
          // don't use short codes here as in a sked with a type 2
          // prefix we would never send out prefix/suffix
          msgtype(t + "73", ui->tx5->lineEdit ());
          break;
        }
    }
    if (hisCall != hisBase
        && m_config.type_2_msg_gen () != Configuration::type_2_msg_5_only
        && !eme_short_codes) {
      // cfm we have his full call copied as we could not do this earlier
      t = hisCall + " 73";
      msgtype(t, ui->tx5->lineEdit ());
    }
  } else {
    if (hisCall != hisBase and SpecOp::HOUND != m_config.special_op_id()) {
      if (shortList(hisCall)) {
        // cfm we know his full call with a type 1 tx1 message
        t = hisCall + " " + my_callsign;
        msgtype(t, ui->tx1);
      }
      else if (!eme_short_codes
               && ("MSK144" != m_mode || !m_bShMsgs)) {
        t=hisCall + " 73";
        msgtype(t, ui->tx5->lineEdit ());
      }
    }
  }
  m_rpt=rpt;
  if(SpecOp::HOUND == m_config.special_op_id() and is_compound) ui->tx1->setText("DE " + my_callsign);
}

void MainWindow::TxAgain()
{
  auto_tx_mode(true);
}

void MainWindow::clearDX ()
{
  set_dateTimeQSO (-1);
  if (m_QSOProgress != CALLING) {
    auto_tx_mode (false);
  }
  ui->dxCallEntry->clear ();
  ui->dxGridEntry->clear ();
  m_lastCallsign.clear ();
  m_rptSent.clear ();
  m_rptRcvd.clear ();
  m_qsoStart.clear ();
  m_qsoStop.clear ();
  m_inQSOwith.clear();
  genStdMsgs (QString {});
  if (m_mode=="FT8" and SpecOp::HOUND == m_config.special_op_id()) {
    m_ntx=1;
    ui->txrb1->setChecked(true);
  } else {
    m_ntx=6;
    ui->txrb6->setChecked(true);
  }
  m_QSOProgress = CALLING;
}

void MainWindow::lookup()
{
  QString hisCall {ui->dxCallEntry->text()};
  QString hisgrid0 {ui->dxGridEntry->text()};
  if (!hisCall.size ()) return;
  QFile f {m_config.writeable_data_dir ().absoluteFilePath ("CALL3.TXT")};
  if (f.open (QIODevice::ReadOnly | QIODevice::Text))
    {
      char c[132];
      qint64 n=0;
      for(int i=0; i<999999; i++) {
        n=f.readLine(c,sizeof(c));
        if(n <= 0) {
          if(!hisgrid0.contains(grid_regexp)) {
            ui->dxGridEntry->clear();
          }
          break;
        }
        QString t=QString(c);
        int i1=t.indexOf(",");
        if(t.left(i1)==hisCall) {
          QString hisgrid=t.mid(i1+1,6);
          i1=hisgrid.indexOf(",");
          if(i1>0) {
            hisgrid=hisgrid.mid(0,4);
          } else {
            hisgrid=hisgrid.mid(0,6).toUpper();
          }
          if(hisgrid.left(4)==hisgrid0.left(4) or (hisgrid0.size()==0)) {
            ui->dxGridEntry->setText(hisgrid);
          }
          break;
        }
      }
      f.close();
    }
}

void MainWindow::on_lookupButton_clicked()                    //Lookup button
{
  lookup();
}

void MainWindow::on_addButton_clicked()                       //Add button
{
  if(!ui->dxGridEntry->text ().size ()) {
    MessageBox::warning_message (this, tr ("Add to CALL3.TXT")
                                 , tr ("Please enter a valid grid locator"));
    return;
  }
  m_call3Modified=false;
  QString hisCall=ui->dxCallEntry->text();
  QString hisgrid=ui->dxGridEntry->text();
  QString newEntry=hisCall + "," + hisgrid;

  //  int ret = MessageBox::query_message(this, tr ("Add to CALL3.TXT"),
  //       tr ("Is %1 known to be active on EME?").arg (newEntry));
  //  if(ret==MessageBox::Yes) {
  //    newEntry += ",EME,,";
  //  } else {
  newEntry += ",,,";
  //  }
  
  QFile f1 {m_config.writeable_data_dir ().absoluteFilePath ("CALL3.TXT")};
  if(!f1.open(QIODevice::ReadWrite | QIODevice::Text)) {
    MessageBox::warning_message (this, tr ("Add to CALL3.TXT")
                                 , tr ("Cannot open \"%1\" for read/write: %2")
                                 .arg (f1.fileName ()).arg (f1.errorString ()));
    return;
  }
  if(f1.size()==0) {
    QTextStream out(&f1);
    out << "ZZZZZZ"
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        << Qt::endl
#else
        << endl
#endif
      ;
    f1.seek (0);
  }
  QFile f2 {m_config.writeable_data_dir ().absoluteFilePath ("CALL3.TMP")};
  if(!f2.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
    MessageBox::warning_message (this, tr ("Add to CALL3.TXT")
                                 , tr ("Cannot open \"%1\" for writing: %2")
                                 .arg (f2.fileName ()).arg (f2.errorString ()));
    return;
  }
  {
    QTextStream in(&f1);          //Read from CALL3.TXT
    QTextStream out(&f2);         //Copy into CALL3.TMP
    QString hc=hisCall;
    QString hc1="";
    QString hc2="000000";
    QString s;
    do {
      s=in.readLine();
      hc1=hc2;
      if(s.mid(0,2)=="//") {
        out << s + QChar::LineFeed; //Copy all comment lines
      } else {
        int i1=s.indexOf(",");
        hc2=s.mid(0,i1);
        if(hc>hc1 && hc<hc2) {
          out << newEntry + QChar::LineFeed;
          out << s + QChar::LineFeed;
          m_call3Modified=true;
        } else if(hc==hc2) {
          QString t {tr ("%1\nis already in CALL3.TXT"
                         ", do you wish to replace it?").arg (s)};
          int ret = MessageBox::query_message (this, tr ("Add to CALL3.TXT"), t);
          if(ret==MessageBox::Yes) {
            out << newEntry + QChar::LineFeed;
            m_call3Modified=true;
          }
        } else {
          if(s!="") out << s + QChar::LineFeed;
        }
      }
    } while(!s.isNull());
    if(hc>hc1 && !m_call3Modified) out << newEntry + QChar::LineFeed;
  }

  if(m_call3Modified) {
    auto const& old_path = m_config.writeable_data_dir ().absoluteFilePath ("CALL3.OLD");
    QFile f0 {old_path};
    if (f0.exists ()) f0.remove ();
    f1.copy (old_path);                       // copying as we want to
                                              // preserve symlinks
    f1.open (QFile::WriteOnly | QFile::Text); // truncates
    f2.seek (0);
    f1.write (f2.readAll ());                 // copy contents
    f2.remove ();
  }
}

void MainWindow::msgtype(QString t, QLineEdit* tx)               //msgtype()
{
// Set background colors of the Tx message boxes, depending on message type
  char message[38];
  char msgsent[38];
  QByteArray s=t.toUpper().toLocal8Bit();
  ba2msg(s,message);
  int ichk=1,itype=0;
  gen65_(message,&ichk,msgsent,const_cast<int*>(itone0),&itype,22,22);
  msgsent[22]=0;
  bool text=false;
  bool shortMsg=false;
  if(itype==6) text=true;

//### Check this stuff ###
  if(itype==7 and m_config.enable_VHF_features() and m_mode=="JT65") shortMsg=true;
  if(m_mode=="MSK144" and t.mid(0,1)=="<") text=false;
  if((m_mode=="MSK144" or m_mode=="FT8" or m_mode=="FT4" || "Q65" == m_mode) and
     SpecOp::NA_VHF==m_config.special_op_id()) {
    int i0=t.trimmed().length()-7;
    if(t.mid(i0,3)==" R ") text=false;
  }
  text=false;
//### ... to here ...


  QPalette p(tx->palette());
  if(text) {
    p.setColor(QPalette::Base,"#ffccff");       //pink
  } else {
    if(shortMsg) {
      p.setColor(QPalette::Base,"#66ffff");     //light blue
    } else {
      p.setColor(QPalette::Base,Qt::transparent);
      if ("MSK144" == m_mode && t.count ('<') == 1) {
        p.setColor(QPalette::Base,"#00ffff");   //another light blue
      }
    }
  }
  tx->setPalette(p);

  auto pos  = tx->cursorPosition ();
  tx->setText(t.toUpper());
  tx->setCursorPosition (pos);
}

void MainWindow::on_tx1_editingFinished()                       //tx1 edited
{
  QString t=ui->tx1->text();
  msgtype(t, ui->tx1);
}

void MainWindow::on_tx2_editingFinished()                       //tx2 edited
{
  QString t=ui->tx2->text();
  msgtype(t, ui->tx2);
}

void MainWindow::on_tx3_editingFinished()                       //tx3 edited
{
  QString t=ui->tx3->text();
  msgtype(t, ui->tx3);
}

void MainWindow::on_tx4_editingFinished()                       //tx4 edited
{
  QString t=ui->tx4->text();
  msgtype(t, ui->tx4);
}

void MainWindow::on_tx5_currentTextChanged (QString const& text) //tx5 edited
{
  msgtype(text, ui->tx5->lineEdit ());
}

void MainWindow::on_tx6_editingFinished()                       //tx6 edited
{
  QString t=ui->tx6->text().toUpper();
  if(t.indexOf(" ")>0) {
    QString t1=t.split(" ").at(1);
    QRegExp AZ4("^[A-Z]{1,4}$");
    QRegExp NN3("^[0-9]{1,3}$");
    m_CQtype="CQ";
    if(t1.size()<=4 and t1.contains(AZ4)) m_CQtype="CQ " + t1;
    if(t1.size()<=3 and t1.contains(NN3)) m_CQtype="CQ " + t1;
  }
  msgtype(t, ui->tx6);
}

void MainWindow::on_RoundRobin_currentTextChanged(QString text)
{
  ui->sbTxPercent->setEnabled (text == tr ("Random"));
}


void MainWindow::on_dxCallEntry_textChanged (QString const& call)
{
  m_hisCall = call;
  ui->dxGridEntry->clear();
  statusChanged();
  statusUpdate ();
}

void MainWindow::on_dxCallEntry_editingFinished()
{
  auto const& dxBase = Radio::base_callsign (m_hisCall);
  save_dxbase_(const_cast <char *> ((dxBase + "   ").left (6).toLatin1().constData()),6);
}


void MainWindow::on_dxCallEntry_returnPressed ()
{
  on_lookupButton_clicked();
}

void MainWindow::on_dxGridEntry_textChanged (QString const& grid)
{
  if (ui->dxGridEntry->hasAcceptableInput ()) {
    if (grid != m_hisGrid) {
      m_hisGrid = grid;
      statusUpdate ();
    }
    qint64 nsec = (QDateTime::currentMSecsSinceEpoch()/1000) % 86400;
    double utch=nsec/3600.0;
    int nAz,nEl,nDmiles,nDkm,nHotAz,nHotABetter;
    azdist_(const_cast <char *> ((m_config.my_grid () + "      ").left (6).toLatin1().constData()),
            const_cast <char *> ((m_hisGrid + "      ").left (6).toLatin1().constData()),&utch,
            &nAz,&nEl,&nDmiles,&nDkm,&nHotAz,&nHotABetter,6,6);
    QString t;
    int nd=nDkm;
    if(m_config.miles()) nd=nDmiles;
    if(m_mode=="MSK144") {
      if(nHotABetter==0)t = t.asprintf("Az: %d   B: %d   El: %d   %d",nAz,nHotAz,nEl,nd);
      if(nHotABetter!=0)t = t.asprintf("Az: %d   A: %d   El: %d   %d",nAz,nHotAz,nEl,nd);
    } else {
      t = t.asprintf("Az: %d        %d",nAz,nd);
    }
    if(m_config.miles()) t += " mi";
    if(!m_config.miles()) t += " km";
    ui->labAz->setText (t);
  } else {
    if (m_hisGrid.size ()) {
      m_hisGrid.clear ();
      ui->labAz->clear ();
      statusUpdate ();
    }
  }
}

void MainWindow::on_genStdMsgsPushButton_clicked()         //genStdMsgs button
{
  genStdMsgs(m_rpt);
}

void MainWindow::cease_auto_Tx_after_QSO ()
{
  if (SpecOp::FOX != m_config.special_op_id ()
      && ui->cbAutoSeq->isVisible () && ui->cbAutoSeq->isEnabled () && ui->cbAutoSeq->isChecked ())
    {
      // ensure that auto Tx is disabled even if disable Tx
      // on 73 is not checked, unless in Fox mode where it is allowed
      // to be a robot.
      auto_tx_mode (false);
    }
}

void MainWindow::on_logQSOButton_clicked()                 //Log QSO button
{
  cease_auto_Tx_after_QSO ();

  if (!m_hisCall.size ()) {
    MessageBox::warning_message (this, tr ("Warning:  DX Call field is empty."));
  }
  // m_dateTimeQSOOn should really already be set but we'll ensure it gets set to something just in case
  if (!m_dateTimeQSOOn.isValid ()) {
    m_dateTimeQSOOn = QDateTime::currentDateTimeUtc();
  }
  auto dateTimeQSOOff = QDateTime::currentDateTimeUtc();
  if (dateTimeQSOOff < m_dateTimeQSOOn) dateTimeQSOOff = m_dateTimeQSOOn;
  QString grid=m_hisGrid;
  if(grid=="....") grid="";
  
  switch( m_config.special_op_id() )
    {
      case SpecOp::NA_VHF:
        m_xSent=m_config.my_grid().left(4);
        m_xRcvd=m_hisGrid;
        break;
      case SpecOp::EU_VHF:
        m_rptSent=m_xSent.split(" ").at(0).left(2);
        m_rptRcvd=m_xRcvd.split(" ").at(0).left(2);
        if(m_xRcvd.split(" ").size()>=2) m_hisGrid=m_xRcvd.split(" ").at(1);
        grid=m_hisGrid;
        ui->dxGridEntry->setText(grid);
        break;
      case SpecOp::FIELD_DAY:
        m_rptSent=m_xSent.split(" ").at(0);
        m_rptRcvd=m_xRcvd.split(" ").at(0);
        break;
      case SpecOp::RTTY:
        m_rptSent=m_xSent.split(" ").at(0);
        m_rptRcvd=m_xRcvd.split(" ").at(0);
        break;
      case SpecOp::WW_DIGI:
        m_xSent=m_config.my_grid().left(4);
        m_xRcvd=m_hisGrid;
        break;
      default: break;
    }

  m_logDlg->initLogQSO (m_hisCall, grid, m_mode, m_rptSent, m_rptRcvd,
                        m_dateTimeQSOOn, dateTimeQSOOff, m_freqNominal +
                        ui->TxFreqSpinBox->value(), m_noSuffix, m_xSent, m_xRcvd);
  m_inQSOwith="";
}

void MainWindow::acceptQSO (QDateTime const& QSO_date_off, QString const& call, QString const& grid
                            , Frequency dial_freq, QString const& mode
                            , QString const& rpt_sent, QString const& rpt_received
                            , QString const& tx_power, QString const& comments
                            , QString const& name, QDateTime const& QSO_date_on, QString const& operator_call
                            , QString const& my_call, QString const& my_grid
                            , QString const& exchange_sent, QString const& exchange_rcvd
                            , QString const& propmode, QByteArray const& ADIF)
{
  QString date = QSO_date_on.toString("yyyyMMdd");
  if (!m_logBook.add (call, grid, m_config.bands()->find(dial_freq), mode, ADIF))
    {
      MessageBox::warning_message (this, tr ("Log file error"),
                                   tr ("Cannot open \"%1\"").arg (m_logBook.path ()));
    }

  m_messageClient->qso_logged (QSO_date_off, call, grid, dial_freq, mode, rpt_sent, rpt_received
                               , tx_power, comments, name, QSO_date_on, operator_call, my_call, my_grid
                               , exchange_sent, exchange_rcvd, propmode);
  m_messageClient->logged_ADIF (ADIF);

  // Log to N1MM Logger
  if (m_config.broadcast_to_n1mm () && m_config.valid_n1mm_info ())
    {
      QUdpSocket sock;
      if (-1 == sock.writeDatagram (ADIF + " <eor>"
                                    , QHostAddress {m_config.n1mm_server_name ()}
                                    , m_config.n1mm_server_port ()))
        {
          MessageBox::warning_message (this, tr ("Error sending log to N1MM"),
                                       tr ("Write returned \"%1\"").arg (sock.errorString ()));
        }
    }

  if(m_config.clear_DX () and SpecOp::HOUND != m_config.special_op_id()) clearDX ();
  m_dateTimeQSOOn = QDateTime {};
  auto special_op = m_config.special_op_id ();
  if (SpecOp::NONE < special_op && special_op < SpecOp::FOX) { 
    ui->sbSerialNumber->setValue(ui->sbSerialNumber->value() + 1);
  }

  m_xSent.clear ();
  m_xRcvd.clear ();
}

qint64 MainWindow::nWidgets(QString t)
{
  Q_ASSERT(t.length()==N_WIDGETS);
  qint64 n=0;
  for(int i=0; i<N_WIDGETS; i++) {
    n=n + n + t.mid(i,1).toInt();
  }
  return n;
}

void MainWindow::displayWidgets(qint64 n)
{
  /* See text file "displayWidgets.txt" for widget numbers */
  qint64 j=qint64(1)<<(N_WIDGETS-1);
  bool b;
  for(int i=0; i<N_WIDGETS; i++) {
    b=(n&j) != 0;    
    if(i==0) ui->txFirstCheckBox->setVisible(b);
    if(i==1) ui->TxFreqSpinBox->setVisible(b);
    if(i==2) ui->RxFreqSpinBox->setVisible(b);
    if(i==3) ui->sbFtol->setVisible(b);
    if(i==4) ui->rptSpinBox->setVisible(b);
    if(i==5) ui->sbTR->setVisible(b);
    if(i==6) {
      ui->sbCQTxFreq->setVisible (b);
      ui->cbCQTx->setVisible (b);
      auto is_compound = m_config.my_callsign () != m_baseCall;
      ui->cbCQTx->setEnabled (b && (!is_compound || shortList (m_config.my_callsign ())));
    }
    if(i==7) ui->cbShMsgs->setVisible(b);
    if(i==8) ui->cbFast9->setVisible(b);
    if(i==9) ui->cbAutoSeq->setVisible(b);
    if(i==10) ui->cbTx6->setVisible(b);
    // if(i==11) ui->pbTxMode->setVisible(b);
    if(i==12) ui->pbR2T->setVisible(b);
    if(i==13) ui->pbT2R->setVisible(b);
    if(i==14) ui->cbHoldTxFreq->setVisible(b);
    if(i==15) ui->sbSubmode->setVisible(b);
    if(i==16) ui->syncSpinBox->setVisible(b);
    if(i==17) ui->WSPR_controls_widget->setVisible(b);
    if(i==18) ui->ClrAvgButton->setVisible(b);
    if(i==19) ui->actionQuickDecode->setEnabled(b);
    if(i==19) ui->actionMediumDecode->setEnabled(b);
    if(i==19) ui->actionDeepestDecode->setEnabled(b);
    if(i==20) ui->actionInclude_averaging->setVisible (b);
    if(i==21) ui->actionInclude_correlation->setVisible (b);
    if(i==22) {
      if(!b && m_echoGraph->isVisible())  m_echoGraph->hide();
    }
    if(i==23) ui->cbSWL->setVisible(b);
    if(i==24) ui->actionEnable_AP_FT8->setVisible (b);
    if(i==25) ui->actionEnable_AP_JT65->setVisible (b);
    if(i==26) ui->actionEnable_AP_DXcall->setVisible (b);
    if(i==27) ui->cbFirst->setVisible(b);
    // if(i==28) ui->labNextCall->setVisible(b);
    if(i==29) ui->measure_check_box->setVisible(b);
    if(i==30) ui->labDXped->setVisible(b);
    if(i==31) ui->cbRxAll->setVisible(b);
    if(i==32) ui->cbCQonly->setVisible(b);
    if(i==33) ui->sbTR_FST4W->setVisible(b);
    if (34 == i)                // adjust the stacked widget
      {
        ui->opt_controls_stack->setCurrentIndex (b ? 1 : 0);
        ui->sbF_Low->setVisible(b);
      }
    if(i==35) ui->sbF_High->setVisible(b);
    if(i==36) ui->actionAuto_Clear_Avg->setVisible (b);
    if(i==37) ui->sbMaxDrift->setVisible(b);
    j=j>>1;
  }
  ui->pbBestSP->setVisible(m_mode=="FT4");
  b=false;
  if(m_mode=="FT4" or m_mode=="FT8" || "Q65" == m_mode) {
  b=SpecOp::EU_VHF==m_config.special_op_id() or 
    ( SpecOp::RTTY==m_config.special_op_id() and
      (m_config.RTTY_Exchange()=="DX" or m_config.RTTY_Exchange()=="#") );
  }
  if(m_mode=="MSK144") b=SpecOp::EU_VHF==m_config.special_op_id();
  ui->sbSerialNumber->setVisible(b);
  m_lastCallsign.clear ();     // ensures Tx5 is updated for new modes
  b=m_mode.startsWith("FST4");
  ui->sbNB->setVisible(b);
  genStdMsgs (m_rpt, true);
}

void MainWindow::on_actionFST4_triggered()
{
  m_mode="FST4";
  m_mode="FST4";
  ui->actionFST4->setChecked(true);
  m_bFast9=false;
  m_bFastMode=false;
  m_fastGraph->hide();
  m_wideGraph->show();
  m_nsps=6912;                   //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize(m_FFTSize);
  ui->lh_decodes_title_label->setText(tr ("Band Activity"));
  ui->rh_decodes_title_label->setText(tr ("Rx Frequency"));
  WSPR_config(false);
  if(m_config.single_decode()) {
//                           01234567890123456789012345678901234567
    displayWidgets(nWidgets("11111100010011100001000000010000000000"));
    m_wideGraph->setSingleDecode(true);
  } else {
    displayWidgets(nWidgets("11101100010011100001000000010000001100"));
    m_wideGraph->setSingleDecode(false);
    ui->sbFtol->setValue(20);
  }
  setup_status_bar(false);
  ui->cbAutoSeq->setChecked(true);
  m_wideGraph->setMode(m_mode);
  m_wideGraph->setPeriod(m_TRperiod,6912);
  m_wideGraph->setRxFreq(ui->RxFreqSpinBox->value());
  m_wideGraph->setTol(ui->sbFtol->value());
  m_wideGraph->setTxFreq(ui->TxFreqSpinBox->value());
  m_wideGraph->setFST4_FreqRange(ui->sbF_Low->value(),ui->sbF_High->value());
  chk_FST4_freq_range();
  switch_mode (Modes::FST4);
  m_wideGraph->setMode(m_mode);
  ui->sbTR->values ({15, 30, 60, 120, 300, 900, 1800});
  on_sbTR_valueChanged (ui->sbTR->value());
  statusChanged();
  m_bOK_to_chk=true;
  chk_FST4_freq_range();
}

void MainWindow::on_actionFST4W_triggered()
{
  m_mode="FST4W";
  ui->actionFST4W->setChecked(true);
  m_bFast9=false;
  m_bFastMode=false;
  m_fastGraph->hide();
  m_wideGraph->show();
  m_nsps=6912;                   //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize(m_FFTSize);
  WSPR_config(true);
//                         01234567890123456789012345678901234567
  displayWidgets(nWidgets("00000000000000000101000000000000010000"));
  setup_status_bar(false);
  ui->band_hopping_group_box->setChecked(false);
  ui->band_hopping_group_box->setVisible(false);
  on_sbTR_FST4W_valueChanged (ui->sbTR_FST4W->value ());
  ui->WSPRfreqSpinBox->setMinimum(100);
  ui->WSPRfreqSpinBox->setMaximum(5000);
  m_wideGraph->setMode(m_mode);
  m_wideGraph->setPeriod(m_TRperiod,6912);
  m_wideGraph->setTxFreq(ui->WSPRfreqSpinBox->value());
  m_wideGraph->setRxFreq(ui->sbFST4W_RxFreq->value());
  m_wideGraph->setTol(ui->sbFST4W_FTol->value());
  ui->sbFtol->setValue(100);
  switch_mode (Modes::FST4W);
  statusChanged();
}

void MainWindow::on_actionFT4_triggered()
{
  m_mode="FT4";
  m_TRperiod=7.5;
  bool bVHF=m_config.enable_VHF_features();
  m_bFast9=false;
  m_bFastMode=false;
  WSPR_config(false);
  switch_mode (Modes::FT4);
  m_nsps=6912;
  m_FFTSize = m_nsps/2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=21;
  setup_status_bar (bVHF);
  m_toneSpacing=12000.0/576.0;
  ui->actionFT4->setChecked(true);
  m_wideGraph->setMode(m_mode);
  m_send_RR73=true;
  VHF_features_enabled(bVHF);
  m_fastGraph->hide();
  m_wideGraph->show();
  ui->rh_decodes_headings_label->setText("  UTC   dB   DT Freq    " + tr ("Message"));
  m_wideGraph->setPeriod(m_TRperiod,m_nsps);
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  ui->rh_decodes_title_label->setText(tr ("Rx Frequency"));
  ui->lh_decodes_title_label->setText(tr ("Band Activity"));
  ui->lh_decodes_headings_label->setText( "  UTC   dB   DT Freq    " + tr ("Message"));
//                         01234567890123456789012345678901234567
  displayWidgets(nWidgets("11101000010011100001000000011000100000"));
  ui->txrb2->setEnabled(true);
  ui->txrb4->setEnabled(true);
  ui->txrb5->setEnabled(true);
  ui->txrb6->setEnabled(true);
  ui->txb2->setEnabled(true);
  ui->txb4->setEnabled(true);
  ui->txb5->setEnabled(true);
  ui->txb6->setEnabled(true);
  ui->txFirstCheckBox->setEnabled(true);
  chkFT4();
  statusChanged();
}

void MainWindow::on_actionFT8_triggered()
{
  m_mode="FT8";
  bool bVHF=m_config.enable_VHF_features();
  m_bFast9=false;
  m_bFastMode=false;
  WSPR_config(false);
  switch_mode (Modes::FT8);
  m_nsps=6912;
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=50;
  setup_status_bar (bVHF);
  m_toneSpacing=0.0;                   //???
  ui->actionFT8->setChecked(true);     //???
  m_wideGraph->setMode(m_mode);
  VHF_features_enabled(bVHF);
  ui->cbAutoSeq->setChecked(true);
  m_TRperiod=15.0;
  m_fastGraph->hide();
  m_wideGraph->show();
  ui->rh_decodes_headings_label->setText("  UTC   dB   DT Freq    " + tr ("Message"));
  m_wideGraph->setPeriod(m_TRperiod,m_nsps);
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  ui->rh_decodes_title_label->setText(tr ("Rx Frequency"));
  if(SpecOp::FOX==m_config.special_op_id()) {
    ui->lh_decodes_title_label->setText(tr ("Stations calling DXpedition %1").arg (m_config.my_callsign()));
    ui->lh_decodes_headings_label->setText( "Call         Grid   dB  Freq   Dist Age Continent");
  } else {
    ui->lh_decodes_title_label->setText(tr ("Band Activity"));
    ui->lh_decodes_headings_label->setText( "  UTC   dB   DT Freq    " + tr ("Message"));
  }
//                         01234567890123456789012345678901234567
  displayWidgets(nWidgets("11101000010011100001000010011000100000"));
  ui->txrb2->setEnabled(true);
  ui->txrb4->setEnabled(true);
  ui->txrb5->setEnabled(true);
  ui->txrb6->setEnabled(true);
  ui->txb2->setEnabled(true);
  ui->txb4->setEnabled(true);
  ui->txb5->setEnabled(true);
  ui->txb6->setEnabled(true);
  ui->txFirstCheckBox->setEnabled(true);
  ui->cbAutoSeq->setEnabled(true);
  if(SpecOp::FOX==m_config.special_op_id()) {
    ui->txFirstCheckBox->setChecked(true);
    ui->txFirstCheckBox->setEnabled(false);
    ui->cbHoldTxFreq->setChecked(true);
    ui->cbAutoSeq->setEnabled(false);
    ui->tabWidget->setCurrentIndex(1);
    ui->TxFreqSpinBox->setValue(300);
  //                         01234567890123456789012345678901234567
    displayWidgets(nWidgets("11101000010011100001000000000010000000"));
    ui->labDXped->setText(tr ("Fox"));
    on_fox_log_action_triggered();
  }
  if(SpecOp::HOUND == m_config.special_op_id()) {
    ui->txFirstCheckBox->setChecked(false);
    ui->txFirstCheckBox->setEnabled(false);
    ui->cbAutoSeq->setEnabled(false);
    ui->tabWidget->setCurrentIndex(0);
    ui->cbHoldTxFreq->setChecked(true);
    //                       01234567890123456789012345678901234567
    displayWidgets(nWidgets("11101000010011000001000000000011000000"));
    ui->labDXped->setText(tr ("Hound"));
    ui->txrb1->setChecked(true);
    ui->txrb2->setEnabled(false);
    ui->txrb4->setEnabled(false);
    ui->txrb5->setEnabled(false);
    ui->txrb6->setEnabled(false);
    ui->txb2->setEnabled(false);
    ui->txb4->setEnabled(false);
    ui->txb5->setEnabled(false);
    ui->txb6->setEnabled(false);
  }

  if (SpecOp::NONE < m_config.special_op_id () && SpecOp::FOX > m_config.special_op_id ()) {
    QString t0="";
    if(SpecOp::NA_VHF==m_config.special_op_id()) t0+="NA VHF";
    if(SpecOp::EU_VHF==m_config.special_op_id()) t0+="EU VHF";
    if(SpecOp::FIELD_DAY==m_config.special_op_id()) t0+="Field Day";
    if(SpecOp::RTTY==m_config.special_op_id()) t0+="RTTY";
    if(SpecOp::WW_DIGI==m_config.special_op_id()) t0+="WW_DIGI";
    if(t0=="") {
      ui->labDXped->setVisible(false);
    } else {
      ui->labDXped->setVisible(true);
      ui->labDXped->setText(t0);
    }
    on_contest_log_action_triggered();
  }

  if((SpecOp::FOX==m_config.special_op_id() or SpecOp::HOUND==m_config.special_op_id()) and !m_config.split_mode() and !m_bWarnedSplit) {
    QString errorMsg;
    MessageBox::critical_message (this,
       "Operation in FT8 DXpedition mode normally requires\n"
       " *Split* rig control (either *Rig* or *Fake It* on\n"
       "the *Settings | Radio* tab.)", errorMsg);
    m_bWarnedSplit=true;
  }
  statusChanged();
}

void MainWindow::on_actionJT4_triggered()
{
  m_mode="JT4";
  bool bVHF=m_config.enable_VHF_features();
  WSPR_config(false);
  switch_mode (Modes::JT4);
  m_TRperiod=60.0;
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  m_nsps=6912;                   //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=176;
  if(m_config.decode_at_52s()) m_hsymStop=184;
  m_toneSpacing=0.0;
  ui->actionJT4->setChecked(true);
  VHF_features_enabled(true);
  m_wideGraph->setPeriod(m_TRperiod,m_nsps);
  m_wideGraph->setMode(m_mode);
  m_bFastMode=false;
  m_bFast9=false;
  setup_status_bar (bVHF);
  ui->sbSubmode->setMaximum(6);
  ui->lh_decodes_title_label->setText(tr ("Single-Period Decodes"));
  ui->rh_decodes_title_label->setText(tr ("Average Decodes"));
  ui->lh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
  ui->rh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
  if(bVHF) {
    ui->sbSubmode->setValue(m_nSubMode);
  } else {
    ui->sbSubmode->setValue(0);
  }
  if(bVHF) {
    //                       01234567890123456789012345678901234567
    displayWidgets(nWidgets("11111001001011011011110000000000000000"));
  } else {
    displayWidgets(nWidgets("11101000000011000011000000000000000000"));
  }
  fast_config(false);
  statusChanged();
}

void MainWindow::on_actionJT9_triggered()
{
  m_mode="JT9";
  bool bVHF=m_config.enable_VHF_features();
  m_bFast9=ui->cbFast9->isChecked();
  m_bFastMode=m_bFast9;
  WSPR_config(false);
  switch_mode (Modes::JT9);
  m_nsps=6912;
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=173;
  if(m_config.decode_at_52s()) m_hsymStop=179;
  setup_status_bar (bVHF);
  m_toneSpacing=0.0;
  ui->actionJT9->setChecked(true);
  m_wideGraph->setMode(m_mode);
  VHF_features_enabled(bVHF);
  if(m_nSubMode>=4 and bVHF) {
    ui->cbFast9->setEnabled(true);
  } else {
    ui->cbFast9->setEnabled(false);
    ui->cbFast9->setChecked(false);
  }
  ui->sbSubmode->setMaximum(7);
  if(m_bFast9) {
    ui->sbTR->values ({5, 10, 15, 30});
    on_sbTR_valueChanged (ui->sbTR->value());
    m_wideGraph->hide();
    m_fastGraph->showNormal();
    ui->TxFreqSpinBox->setValue(700);
    ui->RxFreqSpinBox->setValue(700);
    ui->lh_decodes_headings_label->setText("  UTC   dB    T Freq    " + tr ("Message"));
    ui->rh_decodes_headings_label->setText("  UTC   dB    T Freq    " + tr ("Message"));
  } else {
    ui->cbAutoSeq->setChecked(false);
    if (m_mode != "FST4")
      {
        m_TRperiod=60.0;
        ui->lh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
        ui->rh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
      }
  }
  m_wideGraph->setPeriod(m_TRperiod,m_nsps);
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  ui->lh_decodes_title_label->setText(tr ("Band Activity"));
  ui->rh_decodes_title_label->setText(tr ("Rx Frequency"));
  if(bVHF) {
    //                       01234567890123456789012345678901234567
    displayWidgets(nWidgets("11111010100011111001000000000000000000"));
  } else {
    displayWidgets(nWidgets("11101000000011100001000000000000100000"));
  }
  fast_config(m_bFastMode);
  ui->cbAutoSeq->setVisible(m_bFast9);
  statusChanged();
}

void MainWindow::on_actionJT65_triggered()
{
  on_actionJT9_triggered();
  m_mode="JT65";
  bool bVHF=m_config.enable_VHF_features();
  WSPR_config(false);
  switch_mode (Modes::JT65);
  m_TRperiod=60.0;
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);   // TODO - not thread safe
  m_nsps=6912;                   //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=174;
  if(m_config.decode_at_52s()) m_hsymStop=183;
  m_toneSpacing=0.0;
  ui->actionJT65->setChecked(true);
  VHF_features_enabled(bVHF);
  m_wideGraph->setPeriod(m_TRperiod,m_nsps);
  m_wideGraph->setMode(m_mode);
  m_wideGraph->setRxFreq(ui->RxFreqSpinBox->value());
  m_wideGraph->setTol(ui->sbFtol->value());
  m_wideGraph->setTxFreq(ui->TxFreqSpinBox->value());
  setup_status_bar (bVHF);
  m_bFastMode=false;
  m_bFast9=false;
  ui->sbSubmode->setMaximum(2);
  if(bVHF) {
    ui->sbSubmode->setValue(m_nSubMode);
    ui->lh_decodes_title_label->setText(tr ("Single-Period Decodes"));
    ui->rh_decodes_title_label->setText(tr ("Average Decodes"));
  } else {
    ui->sbSubmode->setValue(0);
    ui->lh_decodes_title_label->setText(tr ("Band Activity"));
    ui->rh_decodes_title_label->setText(tr ("Rx Frequency"));
  }
  if(bVHF) {
    //                       01234567890123456789012345678901234567
    displayWidgets(nWidgets("11111001000011011010110001000000000000"));
  } else {
    displayWidgets(nWidgets("11101000000011100001000000000000100000"));
  }
  fast_config(false);
  if(ui->cbShMsgs->isChecked()) {
    ui->cbAutoSeq->setChecked(false);
    ui->cbAutoSeq->setVisible(false);
  }
  statusChanged();
}

void MainWindow::on_actionQ65_triggered()
{
  m_mode="Q65";
  ui->actionQ65->setChecked(true);
  switch_mode(Modes::Q65);
  fast_config(false);
  WSPR_config(false);
  setup_status_bar(true);
//  ui->actionQuickDecode->setChecked(true);
  m_nsps=6912;                   //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize(m_FFTSize);
  m_hsymStop=49;
  ui->sbTR->values ({15, 30, 60, 120, 300});
  on_sbTR_valueChanged (ui->sbTR->value());
  ui->sbSubmode->setValue(m_nSubMode);
  QString fname {QDir::toNativeSeparators(m_config.temp_dir().absoluteFilePath ("red.dat"))};
  m_wideGraph->setRedFile(fname);
  m_wideGraph->setMode(m_mode);
  m_wideGraph->setPeriod(m_TRperiod,6912);
  m_wideGraph->setTol(ui->sbFtol->value());
  m_wideGraph->setRxFreq(ui->RxFreqSpinBox->value());
  m_wideGraph->setTxFreq(ui->TxFreqSpinBox->value());
  switch_mode (Modes::Q65);
//                         01234567890123456789012345678901234567
  displayWidgets(nWidgets("11111101011011010011100000010000000011"));
  ui->labDXped->setText("");
  ui->lh_decodes_title_label->setText(tr ("Single-Period Decodes"));
  ui->rh_decodes_title_label->setText(tr ("Average Decodes"));
  ui->lh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
  ui->rh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
  statusChanged();

  if (SpecOp::NONE < m_config.special_op_id () && SpecOp::FOX > m_config.special_op_id ()) {
    QString t0="";
    if(SpecOp::NA_VHF==m_config.special_op_id()) t0="NA VHF";
    if(SpecOp::EU_VHF==m_config.special_op_id()) t0="EU VHF";
    if(SpecOp::FIELD_DAY==m_config.special_op_id()) t0="Field Day";
    if(SpecOp::RTTY==m_config.special_op_id()) t0+="RTTY";
    if(SpecOp::WW_DIGI==m_config.special_op_id()) t0+="WW_DIGI";
    if(t0=="") {
      ui->labDXped->setVisible(false);
    } else {
      ui->labDXped->setVisible(true);
      ui->labDXped->setText(t0);
    }
    on_contest_log_action_triggered();
  }

}

void MainWindow::on_actionMSK144_triggered()
{
  if(SpecOp::EU_VHF < m_config.special_op_id()) {
// We are rejecting the requested mode change, so re-check the old mode
    if("FT8"==m_mode) ui->actionFT8->setChecked(true); 
    if("JT4"==m_mode) ui->actionJT4->setChecked(true); 
    if("JT9"==m_mode) ui->actionJT9->setChecked(true); 
    if("JT65"==m_mode) ui->actionJT65->setChecked(true); 
    if("Q65"==m_mode) ui->actionQ65->setChecked(true);
    if("WSPR"==m_mode) ui->actionWSPR->setChecked(true);
    if("Echo"==m_mode) ui->actionEcho->setChecked(true); 
    if("FreqCal"==m_mode) ui->actionFreqCal->setChecked(true);
    if("FST4"==m_mode) ui->actionFST4->setChecked(true);
    if("FST4W"==m_mode) ui->actionFST4W->setChecked(true);
// Make sure that MSK144 is not checked.
    ui->actionMSK144->setChecked(false);
    MessageBox::warning_message (this, tr ("Improper mode"),
       "MSK144 not available if Fox, Hound, Field Day, RTTY, or WW Digi contest is selected.");
    return;
  }
  m_mode="MSK144";
  ui->actionMSK144->setChecked(true);
  switch_mode (Modes::MSK144);
  m_nsps=6;
  m_FFTSize = 7 * 512;
  Q_EMIT FFTSize (m_FFTSize);
  setup_status_bar (true);
  m_toneSpacing=0.0;
  WSPR_config(false);
  VHF_features_enabled(true);
  m_bFastMode=true;
  m_bFast9=false;
  ui->sbTR->values ({5, 10, 15, 30});
  on_sbTR_valueChanged (ui->sbTR->value());
  m_wideGraph->hide();
  m_fastGraph->showNormal();
  ui->TxFreqSpinBox->setValue(1500);
  ui->RxFreqSpinBox->setValue(1500);
  ui->RxFreqSpinBox->setMinimum(1400);
  ui->RxFreqSpinBox->setMaximum(1600);
  ui->RxFreqSpinBox->setSingleStep(10);
  ui->lh_decodes_headings_label->setText("  UTC   dB    T Freq    " + tr ("Message"));
  ui->rh_decodes_headings_label->setText("  UTC   dB    T Freq    " + tr ("Message"));
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  m_fastGraph->setTRPeriod(m_TRperiod);
  ui->lh_decodes_title_label->setText(tr ("Band Activity"));
  ui->rh_decodes_title_label->setText(tr ("Tx Messages"));
  ui->actionMSK144->setChecked(true);
  ui->rptSpinBox->setMinimum(-8);
  ui->rptSpinBox->setMaximum(24);
  ui->rptSpinBox->setValue(0);
  ui->rptSpinBox->setSingleStep(1);
  ui->sbFtol->values ({20, 50, 100, 200});
  //                       01234567890123456789012345678901234567
  displayWidgets(nWidgets("10111111010000000001000100001000000000"));
  fast_config(m_bFastMode);
  statusChanged();

  QString t0="";
  if(SpecOp::NA_VHF==m_config.special_op_id()) t0+="NA VHF";
  if(SpecOp::EU_VHF==m_config.special_op_id()) t0+="EU VHF";
  if(t0=="") {
    ui->labDXped->setVisible(false);
  } else {
    ui->labDXped->setVisible(true);
    ui->labDXped->setText(t0);
    on_contest_log_action_triggered();
  }
}

void MainWindow::on_actionWSPR_triggered()
{
  m_mode="WSPR";
  WSPR_config(true);
  switch_mode (Modes::WSPR);
  m_TRperiod=120.0;
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  m_nsps=6912;                   //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=396;
  m_toneSpacing=12000.0/8192.0;
  setup_status_bar (false);
  ui->actionWSPR->setChecked(true);
  VHF_features_enabled(false);
  ui->WSPRfreqSpinBox->setMinimum(1400);
  ui->WSPRfreqSpinBox->setMaximum(1600);
  m_wideGraph->setPeriod(m_TRperiod,m_nsps);
  m_wideGraph->setMode(m_mode);
  m_bFastMode=false;
  m_bFast9=false;
  ui->TxFreqSpinBox->setValue(ui->WSPRfreqSpinBox->value());
  //                       01234567890123456789012345678901234567
  displayWidgets(nWidgets("00000000000000000101000000000000000000"));
  fast_config(false);
  statusChanged();
}

void MainWindow::on_actionEcho_triggered()
{
  on_actionJT4_triggered();
  m_mode="Echo";
  ui->actionEcho->setChecked(true);
  m_TRperiod=3.0;
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  m_nsps=6912;                        //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=9;
  m_toneSpacing=1.0;
  switch_mode(Modes::Echo);
  setup_status_bar (true);
  m_wideGraph->setMode(m_mode);
  ui->TxFreqSpinBox->setValue(1500);
  ui->TxFreqSpinBox->setEnabled (false);
  if(!m_echoGraph->isVisible()) m_echoGraph->show();
  if (!ui->actionAstronomical_data->isChecked ()) {
    ui->actionAstronomical_data->setChecked (true);
  }
  m_bFastMode=false;
  m_bFast9=false;
  WSPR_config(true);
  ui->lh_decodes_headings_label->setText("   UTC      N   Level    Sig      DF    Width   Q");
  //                       01234567890123456789012345678901234567
  displayWidgets(nWidgets("00000000000000000000001000000000000000"));
  fast_config(false);
  statusChanged();
}

void MainWindow::on_actionFreqCal_triggered()
{
  on_actionJT9_triggered();
  m_mode="FreqCal";
  ui->actionFreqCal->setChecked(true);
  switch_mode(Modes::FreqCal);
  m_wideGraph->setMode(m_mode);
  ui->sbTR->values ({5, 10, 15, 30});
  on_sbTR_valueChanged (ui->sbTR->value());
  m_modulator->setTRPeriod(m_TRperiod); // TODO - not thread safe
  m_detector->setTRPeriod(m_TRperiod);  // TODO - not thread safe
  m_nsps=6912;                        //For symspec only
  m_FFTSize = m_nsps / 2;
  Q_EMIT FFTSize (m_FFTSize);
  m_hsymStop=((int(m_TRperiod/0.288))/8)*8;
  m_frequency_list_fcal_iter = m_config.frequencies ()->begin ();
  ui->RxFreqSpinBox->setValue(1500);
  setup_status_bar (true);
//                               18:15:47      0  1  1500  1550.349     0.100    3.5   10.2
  ui->lh_decodes_headings_label->setText("  UTC      Freq CAL Offset  fMeas       DF     Level   S/N");
  ui->measure_check_box->setChecked (false);
  //                       01234567890123456789012345678901234567
  displayWidgets(nWidgets("00110100000000000000000000000100000000"));
  statusChanged();
}

void MainWindow::switch_mode (Mode mode)
{
  m_fastGraph->setMode(m_mode);
  m_config.frequencies ()->filter (m_config.region (), mode);
  auto const& row = m_config.frequencies ()->best_working_frequency (m_freqNominal);
  ui->bandComboBox->setCurrentIndex (row);
  if (row >= 0) {
    on_bandComboBox_activated (row);
  }
  ui->rptSpinBox->setSingleStep(1);
  ui->rptSpinBox->setMinimum(-50);
  ui->rptSpinBox->setMaximum(49);
  ui->sbFtol->values ({1, 2, 5, 10, 20, 50, 100, 200, 300, 400, 500, 1000});
  ui->sbFST4W_FTol->values({1, 2, 5, 10, 20, 50, 100});
  if(m_mode=="MSK144") {
    ui->RxFreqSpinBox->setMinimum(1400);
    ui->RxFreqSpinBox->setMaximum(1600);
    ui->RxFreqSpinBox->setSingleStep(25);
  } else {
    ui->RxFreqSpinBox->setMinimum(200);
    ui->RxFreqSpinBox->setMaximum(5000);
    ui->RxFreqSpinBox->setSingleStep(1);
  }
  bool b=m_mode=="FreqCal";
  ui->tabWidget->setVisible(!b);
  if(b) {
    ui->DX_controls_widget->setVisible(false);
    ui->rh_decodes_widget->setVisible (false);
    ui->lh_decodes_title_label->setVisible(false);
  }
}

void MainWindow::WSPR_config(bool b)
{
  ui->rh_decodes_widget->setVisible(!b);
  ui->controls_stack_widget->setCurrentIndex (b && m_mode != "Echo" ? 1 : 0);
  ui->QSO_controls_widget->setVisible (!b);
  ui->DX_controls_widget->setVisible (!b);
  ui->WSPR_controls_widget->setVisible (b);
  ui->lh_decodes_title_label->setVisible(!b and ui->cbMenus->isChecked());
  ui->logQSOButton->setVisible(!b);
  ui->DecodeButton->setEnabled(!b);
  bool bFST4W=(m_mode=="FST4W");
  ui->sbTxPercent->setEnabled(!bFST4W or (tr("Random") == ui->RoundRobin->currentText()));
  ui->band_hopping_group_box->setVisible(true);
  ui->RoundRobin->setVisible(bFST4W);
  ui->sbFST4W_RxFreq->setVisible(bFST4W);
  ui->sbFST4W_FTol->setVisible(bFST4W);
  ui->RoundRobin->lineEdit()->setAlignment(Qt::AlignCenter);
  if(b and m_mode!="Echo" and m_mode!="FST4W") {
    QString t="UTC    dB   DT     Freq     Drift  Call          Grid    dBm    ";
    if(m_config.miles()) t += " mi";
    if(!m_config.miles()) t += " km";
    ui->lh_decodes_headings_label->setText(t);
    if (m_config.is_transceiver_online ()) {
      m_config.transceiver_tx_frequency (0); // turn off split
    }
    m_bSimplex = true;
  } else
    {
      m_bSimplex = false;
    }
  enable_DXCC_entity (m_config.DXCC ());  // sets text window proportions and (re)inits the logbook
}

void MainWindow::fast_config(bool b)
{
  m_bFastMode=b;
  ui->TxFreqSpinBox->setEnabled(!b);
  ui->sbTR->setVisible(b);
  if(b and (m_bFast9 or m_mode=="MSK144")) {
    m_wideGraph->hide();
    m_fastGraph->showNormal();
  } else {
    m_wideGraph->showNormal();
    m_fastGraph->hide();
  }
}

void MainWindow::on_TxFreqSpinBox_valueChanged(int n)
{
  m_wideGraph->setTxFreq(n);
//  if (ui->cbHoldTxFreq->isChecked ()) ui->RxFreqSpinBox->setValue(n);
  if(m_mode!="MSK144") {
    Q_EMIT transmitFrequency (n - m_XIT);
  }

  if(m_mode=="Q65") {
    if(((m_nSubMode==4 && m_TRperiod==60.0) || (m_nSubMode==3 && m_TRperiod==30.0) ||
       (m_nSubMode==2 && m_TRperiod==15.0)) && ui->TxFreqSpinBox->value()!=700) {
      ui->TxFreqSpinBox->setStyleSheet("QSpinBox{background-color:red}");
    } else {
      ui->TxFreqSpinBox->setStyleSheet("");
    }
  }

  statusUpdate ();
}

void MainWindow::on_RxFreqSpinBox_valueChanged(int n)
{
  m_wideGraph->setRxFreq(n);
  if (m_mode == "FreqCal") {
    setRig ();
  }
  statusUpdate ();
}

void MainWindow::on_sbF_Low_valueChanged(int n)
{
  m_wideGraph->setFST4_FreqRange(n,ui->sbF_High->value());
  chk_FST4_freq_range();
}

void MainWindow::on_sbF_High_valueChanged(int n)
{
  m_wideGraph->setFST4_FreqRange(ui->sbF_Low->value(),n);
  chk_FST4_freq_range();
}

void MainWindow::chk_FST4_freq_range()
{
  if(!m_bOK_to_chk) return;
  if(ui->sbF_Low->value() < m_wideGraph->nStartFreq()) ui->sbF_Low->setValue(m_wideGraph->nStartFreq());
  if(ui->sbF_High->value() > m_wideGraph->Fmax()) {
    int n=m_wideGraph->Fmax()/100;
    ui->sbF_High->setValue(100*n);
  }
  int maxDiff=2000;
  if(m_TRperiod==120) maxDiff=1000;
  if(m_TRperiod==300) maxDiff=400;
  if(m_TRperiod>=900) maxDiff=200;
  int diff=ui->sbF_High->value() - ui->sbF_Low->value();

  if(diff<100 or diff>maxDiff) {
    ui->sbF_Low->setStyleSheet("QSpinBox { color: white; background-color: red; }");
    ui->sbF_High->setStyleSheet("QSpinBox { color: white; background-color: red; }");
  } else {
    ui->sbF_Low->setStyleSheet("");
    ui->sbF_High->setStyleSheet("");
  }
}

void MainWindow::on_actionQuickDecode_toggled (bool checked)
{
  m_ndepth ^= (-checked ^ m_ndepth) & 0x00000001;
}

void MainWindow::on_actionMediumDecode_toggled (bool checked)
{
  m_ndepth ^= (-checked ^ m_ndepth) & 0x00000002;
}

void MainWindow::on_actionDeepestDecode_toggled (bool checked)
{
  m_ndepth ^= (-checked ^ m_ndepth) & 0x00000003;
}

void MainWindow::on_actionInclude_averaging_toggled (bool checked)
{
  m_ndepth ^= (-checked ^ m_ndepth) & 0x00000010;
}

void MainWindow::on_actionInclude_correlation_toggled (bool checked)
{
  m_ndepth ^= (-checked ^ m_ndepth) & 0x00000020;
}

void MainWindow::on_actionEnable_AP_DXcall_toggled (bool checked)
{
  m_ndepth ^= (-checked ^ m_ndepth) & 0x00000040;
}

void MainWindow::on_actionAuto_Clear_Avg_toggled (bool checked)
{
  m_ndepth ^= (-checked ^ m_ndepth) & 0x00000080;
}

void MainWindow::on_actionErase_ALL_TXT_triggered()          //Erase ALL.TXT
{
  int ret = MessageBox::query_message (this, tr ("Confirm Erase"),
                                         tr ("Are you sure you want to erase file ALL.TXT?"));
  if(ret==MessageBox::Yes) {
    QFile f {m_config.writeable_data_dir ().absoluteFilePath ("ALL.TXT")};
    f.remove();
    m_RxLog=1;
  }
}

void MainWindow::on_reset_cabrillo_log_action_triggered ()
{
  if (MessageBox::Yes == MessageBox::query_message (this, tr ("Confirm Reset"),
                                                    tr ("Are you sure you want to erase your contest log?"),
                                                    tr ("Doing this will remove all QSO records for the current contest. "
                                                        "They will be kept in the ADIF log file but will not be available "
                                                        "for export in your Cabrillo log.")))
    {
      if(m_config.RTTY_Exchange()!="SCC") ui->sbSerialNumber->setValue(1);
      m_logBook.contest_log ()->reset ();
    }
}

void MainWindow::on_actionExport_Cabrillo_log_triggered()
{
  if (QDialog::Accepted == ExportCabrillo {m_settings, &m_config, m_logBook.contest_log ()}.exec())
    {
      MessageBox::information_message (this, tr ("Cabrillo Log saved"));
    }
}


void MainWindow::on_actionErase_wsjtx_log_adi_triggered()
{
  int ret = MessageBox::query_message (this, tr ("Confirm Erase"),
                                       tr ("Are you sure you want to erase file wsjtx_log.adi?"));
  if(ret==MessageBox::Yes) {
    QFile f {m_config.writeable_data_dir ().absoluteFilePath ("wsjtx_log.adi")};
    f.remove();
  }
}

void MainWindow::on_actionErase_WSPR_hashtable_triggered()
{
  int ret = MessageBox::query_message(this, tr ("Confirm Erase"),
            tr ("Are you sure you want to erase the WSPR hashtable?"));
  if(ret==MessageBox::Yes) {
    QFile f {m_config.writeable_data_dir().absoluteFilePath("hashtable.txt")};
    f.remove();
  }
}


void MainWindow::on_actionOpen_log_directory_triggered ()
{
  QDesktopServices::openUrl (QUrl::fromLocalFile (m_config.writeable_data_dir ().absolutePath ()));
}

void MainWindow::on_bandComboBox_currentIndexChanged (int index)
{
  auto const& frequencies = m_config.frequencies ();
  auto const& source_index = frequencies->mapToSource (frequencies->index (index, FrequencyList_v2::frequency_column));
  Frequency frequency {m_freqNominal};
  if (source_index.isValid ())
    {
      frequency = frequencies->frequency_list ()[source_index.row ()].frequency_;
    }

  // Lookup band
  auto const& band  = m_config.bands ()->find (frequency);
  ui->bandComboBox->setCurrentText (band.size () ? band : m_config.bands ()->oob ());
  displayDialFrequency ();
}

void MainWindow::on_bandComboBox_editTextChanged (QString const& text)
{
  if (text.size () && m_config.bands ()->oob () != text)
    {
      ui->bandComboBox->lineEdit ()->setStyleSheet ({});
    }
  else
    {
      ui->bandComboBox->lineEdit ()->setStyleSheet ("QLineEdit {color: yellow; background-color : red;}");
    }
}

void MainWindow::on_bandComboBox_activated (int index)
{
  auto const& frequencies = m_config.frequencies ();
  auto const& source_index = frequencies->mapToSource (frequencies->index (index, FrequencyList_v2::frequency_column));
  Frequency frequency {m_freqNominal};
  if (source_index.isValid ())
    {
      frequency = frequencies->frequency_list ()[source_index.row ()].frequency_;
    }
  m_bandEdited = true;
  band_changed (frequency);
  m_wideGraph->setRxBand (m_config.bands ()->find (frequency));
}

void MainWindow::band_changed (Frequency f)
{
  // Set the attenuation value if options are checked
  if (m_config.pwrBandTxMemory() && !m_tune) {
    auto const&curBand = ui->bandComboBox->currentText();
    if (m_pwrBandTxMemory.contains(curBand)) {
      ui->outAttenuation->setValue(m_pwrBandTxMemory[curBand].toInt());
    }
    else {
      m_pwrBandTxMemory[curBand] = ui->outAttenuation->value();
    }
  }

  if (m_bandEdited) {
    if (m_mode!="WSPR") { // band hopping preserves auto Tx
      if (f + m_wideGraph->nStartFreq () > m_freqNominal + ui->TxFreqSpinBox->value ()
          || f + m_wideGraph->nStartFreq () + m_wideGraph->fSpan () <=
          m_freqNominal + ui->TxFreqSpinBox->value ()) {
//        qDebug () << "start f:" << m_wideGraph->nStartFreq () << "span:" << m_wideGraph->fSpan () << "DF:" << ui->TxFreqSpinBox->value ();
        // disable auto Tx if "blind" QSY outside of waterfall
        ui->stopTxButton->click (); // halt any transmission
        auto_tx_mode (false);       // disable auto Tx
//        m_send_RR73 = false;        // force user to reassess on new band
      }
    }
    m_lastBand.clear ();
    m_bandEdited = false;
    if (m_config.spot_to_psk_reporter ())
      {
        // Upload any queued spots before changing band
        m_psk_Reporter.sendReport();
      }
    if (!m_transmitting) monitor (true);
    if ("FreqCal" == m_mode)
      {
        m_frequency_list_fcal_iter = m_config.frequencies ()->find (f);
      }
    setRig (f);
    setXIT (ui->TxFreqSpinBox->value ());
  }
}

void MainWindow::enable_DXCC_entity (bool on)
{
  if (on and m_mode!="WSPR" and m_mode!="FST4W" and m_mode!="Echo") {
    //m_logBook.init();                        // re-read the log and cty.dat files
//    ui->gridLayout->setColumnStretch(0,55);  // adjust proportions of text displays
//    ui->gridLayout->setColumnStretch(1,45);
  } else {
//    ui->gridLayout->setColumnStretch(0,0);
//    ui->gridLayout->setColumnStretch(1,0);
  }
  updateGeometry ();
}

void MainWindow::on_rptSpinBox_valueChanged(int n)
{
  int step=ui->rptSpinBox->singleStep();
  if(n%step !=0) {
    n++;
    ui->rptSpinBox->setValue(n);
  }
  m_rpt=QString::number(n);
  int ntx0=m_ntx;
  genStdMsgs(m_rpt);
  m_ntx=ntx0;
  if(m_ntx==1) ui->txrb1->setChecked(true);
  if(m_ntx==2) ui->txrb2->setChecked(true);
  if(m_ntx==3) ui->txrb3->setChecked(true);
  if(m_ntx==4) ui->txrb4->setChecked(true);
  if(m_ntx==5) ui->txrb5->setChecked(true);
  if(m_ntx==6) ui->txrb6->setChecked(true);
  statusChanged();
}

void MainWindow::on_tuneButton_clicked (bool checked)
{
  static bool lastChecked = false;
  if (lastChecked == checked) return;
  lastChecked = checked;
  if (checked && m_tune==false) { // we're starting tuning so remember Tx and change pwr to Tune value
    if (m_config.pwrBandTuneMemory ()) {
      auto const& curBand = ui->bandComboBox->currentText();
      m_pwrBandTxMemory[curBand] = ui->outAttenuation->value(); // remember our Tx pwr
      m_PwrBandSetOK = false;
      if (m_pwrBandTuneMemory.contains(curBand)) {
        ui->outAttenuation->setValue(m_pwrBandTuneMemory[curBand].toInt()); // set to Tune pwr
      }
      m_PwrBandSetOK = true;
    }
  }
  if (m_tune) {
    tuneButtonTimer.start(250);
  } else {
    m_sentFirst73=false;
    itone[0]=0;
    on_monitorButton_clicked (true);
    m_tune=true;
  }
  Q_EMIT tune (checked);
}

void MainWindow::end_tuning ()
{
  on_stopTxButton_clicked ();
  // we're turning off so remember our Tune pwr setting and reset to Tx pwr
  if (m_config.pwrBandTuneMemory() || m_config.pwrBandTxMemory()) {
    auto const& curBand = ui->bandComboBox->currentText();
    m_pwrBandTuneMemory[curBand] = ui->outAttenuation->value(); // remember our Tune pwr
    m_PwrBandSetOK = false;
    ui->outAttenuation->setValue(m_pwrBandTxMemory[curBand].toInt()); // set to Tx pwr
    m_PwrBandSetOK = true;
  }
}

void MainWindow::stop_tuning ()
{
  on_tuneButton_clicked(false);
  ui->tuneButton->setChecked (false);
  m_bTxTime=false;
  m_tune=false;
}

void MainWindow::stopTuneATU()
{
  on_tuneButton_clicked(false);
  m_bTxTime=false;
}

void MainWindow::on_stopTxButton_clicked()                    //Stop Tx
{
  if (m_tune) stop_tuning ();
  if (m_auto and !m_tuneup) auto_tx_mode (false);
  m_btxok=false;
  m_bCallingCQ = false;
  m_bAutoReply = false;         // ready for next
  ui->cbFirst->setStyleSheet ("");
}

void MainWindow::rigOpen ()
{
  update_dynamic_property (ui->readFreq, "state", "warning");
  ui->readFreq->setText ("");
  ui->readFreq->setEnabled (true);
  m_config.transceiver_online ();
  m_config.sync_transceiver (true, true);
}

void MainWindow::on_pbR2T_clicked()
{
  ui->TxFreqSpinBox->setValue(ui->RxFreqSpinBox->value ());
}

void MainWindow::on_pbT2R_clicked()
{
  if (ui->RxFreqSpinBox->isEnabled ())
    {
      ui->RxFreqSpinBox->setValue (ui->TxFreqSpinBox->value ());
    }
}


void MainWindow::on_readFreq_clicked()
{
  if (m_transmitting) return;

  if (m_config.transceiver_online ())
    {
      m_config.sync_transceiver (true, true);
    }
}

void MainWindow::setXIT(int n, Frequency base)
{
  if (m_transmitting && !m_config.tx_QSY_allowed ()) return;
  // If "CQ nnn ..." feature is active, set the proper Tx frequency
  if(m_config.split_mode () && ui->cbCQTx->isEnabled () && ui->cbCQTx->isVisible () &&
     ui->cbCQTx->isChecked())
    {
      if (6 == m_ntx || (7 == m_ntx && m_gen_message_is_cq))
        {
          // All conditions are met, use calling frequency
          base = m_freqNominal / 1000000 * 1000000 + 1000 * ui->sbCQTxFreq->value () + m_XIT;
        }
    }
  if (!base) base = m_freqNominal;
  m_XIT = 0;
  if (!m_bSimplex) {
    // m_bSimplex is false, so we can use split mode if requested
    if (m_config.split_mode () && (!m_config.enable_VHF_features () ||
        m_mode=="FT4" || m_mode == "FT8" || m_mode=="FST4")) {
      // Don't use XIT for VHF & up
      m_XIT=(n/500)*500 - 1500;
    }

    if ((m_monitoring || m_transmitting)
        && m_config.is_transceiver_online ()
        && m_config.split_mode ())
      {
        // All conditions are met, reset the transceiver Tx dial
        // frequency
        m_freqTxNominal = base + m_XIT;
        if (m_astroWidget) m_astroWidget->nominal_frequency (m_freqNominal, m_freqTxNominal);
        m_config.transceiver_tx_frequency (m_freqTxNominal + m_astroCorrection.tx);
      }
  }
  //Now set the audio Tx freq
  Q_EMIT transmitFrequency (ui->TxFreqSpinBox->value () - m_XIT);
}

void MainWindow::setFreq4(int rxFreq, int txFreq)
{
  if (ui->RxFreqSpinBox->isEnabled ()) ui->RxFreqSpinBox->setValue(rxFreq);
  if(m_mode=="WSPR" or m_mode=="FST4W") {
    ui->WSPRfreqSpinBox->setValue(txFreq);
  } else {
    if (ui->TxFreqSpinBox->isEnabled ()) {
      ui->TxFreqSpinBox->setValue(txFreq);
      if ("FT8" == m_mode || "FT4" == m_mode || m_mode=="FST4")
        {
          // we need to regenerate the current transmit waveform for
          // GFSK modulated modes
          if (m_transmitting) m_restart = true;
        }
    }
    else if (m_config.enable_VHF_features ()
             && (Qt::ControlModifier & QApplication::keyboardModifiers ())) {
      // for VHF & up we adjust Tx dial frequency to equalize Tx to Rx
      // when user CTRL+clicks on waterfall
      auto temp = ui->TxFreqSpinBox->value ();
      ui->RxFreqSpinBox->setValue (temp);
      setRig (m_freqNominal + txFreq - temp);
      setXIT (ui->TxFreqSpinBox->value ());
    }
  }
}

void MainWindow::handle_transceiver_update (Transceiver::TransceiverState const& s)
{
  Transceiver::TransceiverState old_state {m_rigState};
  //transmitDisplay (s.ptt ());
  if (s.ptt () // && !m_rigState.ptt ()
      ) { // safe to start audio
                                        // (caveat - DX Lab Suite Commander)
    if (m_tx_when_ready && g_iptt) {    // waiting to Tx and still needed
      int ms_delay=1000*m_config.txDelay();
      if(m_mode=="FT4") ms_delay=20;
      ptt1Timer.start(ms_delay); //Start-of-transmission sequencer delay
      m_tx_when_ready = false;
    }
  }
  m_rigState = s;
  auto old_freqNominal = m_freqNominal;
  if (!old_freqNominal)
    {
      // always take initial rig frequency to avoid start up problems
      // with bogus Tx frequencies
      m_freqNominal = s.frequency ();
    }
  if (old_state.online () == false && s.online () == true)
    {
      // initializing
      on_monitorButton_clicked (!m_config.monitor_off_at_startup ());
    }
  if (s.frequency () != old_state.frequency () || s.split () != m_splitMode)
    {
      m_splitMode = s.split ();
      if (!s.ptt ())
        {
          m_freqNominal = s.frequency () - m_astroCorrection.rx;
          if (old_freqNominal != m_freqNominal)
            {
              m_freqTxNominal = m_freqNominal;
              genCQMsg ();
            }

          if (m_monitoring)
            {
              m_lastMonitoredFrequency = m_freqNominal;
            }
          if (m_lastDialFreq != m_freqNominal &&
              (m_mode != "MSK144"
               || !(ui->cbCQTx->isEnabled () && ui->cbCQTx->isVisible () && ui->cbCQTx->isChecked()))) {
            m_lastDialFreq = m_freqNominal;
            m_secBandChanged=QDateTime::currentMSecsSinceEpoch()/1000;
            pskSetLocal ();
            statusChanged();
            m_wideGraph->setDialFreq(m_freqNominal / 1.e6);
          }
      } else {
        m_freqTxNominal = s.split () ? s.tx_frequency () - m_astroCorrection.tx : s.frequency ();
      }
      if (m_astroWidget) m_astroWidget->nominal_frequency (m_freqNominal, m_freqTxNominal);
  }
  // ensure frequency display is correct
  if (m_astroWidget && old_state.ptt () != s.ptt ()) setRig ();

  displayDialFrequency ();
  update_dynamic_property (ui->readFreq, "state", "ok");
  ui->readFreq->setEnabled (false);
  ui->readFreq->setText (s.split () ? "S" : "");
}

void MainWindow::handle_transceiver_failure (QString const& reason)
{
  update_dynamic_property (ui->readFreq, "state", "error");
  ui->readFreq->setEnabled (true);
  on_stopTxButton_clicked ();
  rigFailure (reason);
}

void MainWindow::rigFailure (QString const& reason)
{
  if (m_first_error)
    {
      // one automatic retry
      QTimer::singleShot (0, this, SLOT (rigOpen ()));
      m_first_error = false;
    }
  else
    {
      if (m_splash && m_splash->isVisible ()) m_splash->hide ();
      m_rigErrorMessageBox.setDetailedText (reason + "\n\nTimestamp: "
#if QT_VERSION >= QT_VERSION_CHECK (5, 8, 0)
                                            + QDateTime::currentDateTimeUtc ().toString (Qt::ISODateWithMs)
#else
                                            + QDateTime::currentDateTimeUtc ().toString ("yyyy-MM-ddTHH:mm:ss.zzzZ")
#endif
                                            );

      // don't call slot functions directly to avoid recursion
      m_rigErrorMessageBox.exec ();
      auto const clicked_button = m_rigErrorMessageBox.clickedButton ();
      if (clicked_button == m_configurations_button)
        {
          ui->menuConfig->exec (QCursor::pos ());
        }
      else
        {
          switch (m_rigErrorMessageBox.standardButton (clicked_button))
            {
            case MessageBox::Ok:
              m_config.select_tab (1);
              QTimer::singleShot (0, this, SLOT (on_actionSettings_triggered ()));
              break;

            case MessageBox::Retry:
              QTimer::singleShot (0, this, SLOT (rigOpen ()));
              break;

            case MessageBox::Cancel:
              QTimer::singleShot (0, this, SLOT (close ()));
              break;

            default: break;     // squashing compile warnings
            }
        }
      m_first_error = true;     // reset
    }
}

void MainWindow::transmit (double snr)
{
  double toneSpacing=0.0;
  if (m_mode == "JT65") {
    if(m_nSubMode==0) toneSpacing=11025.0/4096.0;
    if(m_nSubMode==1) toneSpacing=2*11025.0/4096.0;
    if(m_nSubMode==2) toneSpacing=4*11025.0/4096.0;
    Q_EMIT sendMessage (m_mode, NUM_JT65_SYMBOLS,
           4096.0*12000.0/11025.0, ui->TxFreqSpinBox->value () - m_XIT,
           toneSpacing, m_soundOutput, m_config.audio_output_channel (),
           true, false, snr, m_TRperiod);
  }

  if (m_mode == "FT8") {
//    toneSpacing=12000.0/1920.0;
    toneSpacing=-3;
    if(m_config.x2ToneSpacing()) toneSpacing=2*12000.0/1920.0;
    if(m_config.x4ToneSpacing()) toneSpacing=4*12000.0/1920.0;
    if(SpecOp::FOX==m_config.special_op_id() and !m_tune) toneSpacing=-1;
    Q_EMIT sendMessage (m_mode, NUM_FT8_SYMBOLS,
           1920.0, ui->TxFreqSpinBox->value () - m_XIT,
           toneSpacing, m_soundOutput, m_config.audio_output_channel (),
           true, false, snr, m_TRperiod);
  }

  if (m_mode == "FT4") {
    m_dateTimeSentTx3=QDateTime::currentDateTimeUtc();
    toneSpacing=-2.0;                     //Transmit a pre-computed, filtered waveform.
    Q_EMIT sendMessage (m_mode, NUM_FT4_SYMBOLS,
           576.0, ui->TxFreqSpinBox->value() - m_XIT,
           toneSpacing, m_soundOutput, m_config.audio_output_channel(),
           true, false, snr, m_TRperiod);
  }

  if (m_mode == "FST4" or m_mode == "FST4W") {
    m_dateTimeSentTx3=QDateTime::currentDateTimeUtc();
    toneSpacing=-2.0;                     //Transmit a pre-computed, filtered waveform.
    int nsps=720;
    if(m_TRperiod==30) nsps=1680;
    if(m_TRperiod==60) nsps=3888;
    if(m_TRperiod==120) nsps=8200;
    if(m_TRperiod==300) nsps=21504;
    if(m_TRperiod==900) nsps=66560;
    if(m_TRperiod==1800) nsps=134400;
    int hmod=1;
    if(m_config.x2ToneSpacing()) hmod=2;
    if(m_config.x4ToneSpacing()) hmod=4;
    double dfreq=hmod*12000.0/nsps;
    double f0=ui->WSPRfreqSpinBox->value() - m_XIT;
    if(m_mode=="FST4") f0=ui->TxFreqSpinBox->value() - m_XIT;
    if(!m_tune) f0 += 1.5*dfreq;
    Q_EMIT sendMessage (m_mode, NUM_FST4_SYMBOLS,double(nsps),f0,toneSpacing,
                        m_soundOutput,m_config.audio_output_channel(),
                        true, false, snr, m_TRperiod);
  }

  if (m_mode == "Q65") {
    int nsps=1800;
    if(m_TRperiod==30) nsps=3600;
    if(m_TRperiod==60) nsps=7200;
    if(m_TRperiod==120) nsps=16000;
    if(m_TRperiod==300) nsps=41472;
    int mode65=pow(2.0,double(m_nSubMode));
    toneSpacing=mode65*12000.0/nsps;
//    toneSpacing=-4.0;
    Q_EMIT sendMessage (m_mode, NUM_Q65_SYMBOLS,
           double(nsps), ui->TxFreqSpinBox->value () - m_XIT,
           toneSpacing, m_soundOutput, m_config.audio_output_channel (),
           true, false, snr, m_TRperiod);
  }

  if (m_mode == "JT9") {
    int nsub=pow(2,m_nSubMode);
    int nsps[]={480,240,120,60};
    double sps=m_nsps;
    m_toneSpacing=nsub*12000.0/6912.0;
    if(m_config.x2ToneSpacing()) m_toneSpacing=2.0*m_toneSpacing;
    if(m_config.x4ToneSpacing()) m_toneSpacing=4.0*m_toneSpacing;
    bool fastmode=false;
    if(m_bFast9 and (m_nSubMode>=4)) {
      fastmode=true;
      sps=nsps[m_nSubMode-4];
      m_toneSpacing=12000.0/sps;
    }
    Q_EMIT sendMessage (m_mode, NUM_JT9_SYMBOLS, sps,
                        ui->TxFreqSpinBox->value() - m_XIT, m_toneSpacing,
                        m_soundOutput, m_config.audio_output_channel (),
                        true, fastmode, snr, m_TRperiod);
  }

  if (m_mode == "MSK144") {
    m_nsps=6;
    double f0=1000.0;
    if(!m_bFastMode) {
      m_nsps=192;
      f0=ui->TxFreqSpinBox->value () - m_XIT - 0.5*m_toneSpacing;
    }
    m_toneSpacing=6000.0/m_nsps;
    m_FFTSize = 7 * 512;
    Q_EMIT FFTSize (m_FFTSize);
    int nsym;
    nsym=NUM_MSK144_SYMBOLS;
    if(itone[40] < 0) nsym=40;
    Q_EMIT sendMessage (m_mode, nsym, double(m_nsps), f0, m_toneSpacing,
                        m_soundOutput, m_config.audio_output_channel (),
                        true, true, snr, m_TRperiod);
  }

  if (m_mode == "JT4") {
    if(m_nSubMode==0) toneSpacing=4.375;
    if(m_nSubMode==1) toneSpacing=2*4.375;
    if(m_nSubMode==2) toneSpacing=4*4.375;
    if(m_nSubMode==3) toneSpacing=9*4.375;
    if(m_nSubMode==4) toneSpacing=18*4.375;
    if(m_nSubMode==5) toneSpacing=36*4.375;
    if(m_nSubMode==6) toneSpacing=72*4.375;
    Q_EMIT sendMessage (m_mode, NUM_JT4_SYMBOLS,
           2520.0*12000.0/11025.0, ui->TxFreqSpinBox->value () - m_XIT,
           toneSpacing, m_soundOutput, m_config.audio_output_channel (),
           true, false, snr, m_TRperiod);
  }
  if (m_mode=="WSPR") {
    int nToneSpacing=1;
    if(m_config.x2ToneSpacing()) nToneSpacing=2;
    if(m_config.x4ToneSpacing()) nToneSpacing=4;
    Q_EMIT sendMessage (m_mode, NUM_WSPR_SYMBOLS, 8192.0,
                        ui->TxFreqSpinBox->value() - 1.5 * 12000 / 8192,
                        m_toneSpacing*nToneSpacing, m_soundOutput,
                        m_config.audio_output_channel(),true, false, snr,
                        m_TRperiod);
  }

  if(m_mode=="Echo") {
    //??? should use "fastMode = true" here ???
    Q_EMIT sendMessage (m_mode, 27, 1024.0, 1500.0, 0.0, m_soundOutput,
                        m_config.audio_output_channel(),
                        false, false, snr, m_TRperiod);
  }

// In auto-sequencing mode, stop after 5 transmissions of "73" message.
  if (m_bFastMode || m_bFast9) {
    if (ui->cbAutoSeq->isVisible () && ui->cbAutoSeq->isEnabled () && ui->cbAutoSeq->isChecked ()) {
      if(m_ntx==5) {
        m_nTx73 += 1;
      } else {
        m_nTx73=0;
      }
    }
  }
}

void MainWindow::on_outAttenuation_valueChanged (int a)
{
  QString tt_str;
  qreal dBAttn {a / 10.};       // slider interpreted as dB / 100
  if (m_tune && m_config.pwrBandTuneMemory()) {
    tt_str = tr ("Tune digital gain ");
  } else {
    tt_str = tr ("Transmit digital gain ");
  }
  tt_str += (a ? QString::number (-dBAttn, 'f', 1) : "0") + "dB";
  if (!m_block_pwr_tooltip) {
    QToolTip::showText (QCursor::pos (), tt_str, ui->outAttenuation);
  }
  QString curBand = ui->bandComboBox->currentText();
  if (m_PwrBandSetOK && !m_tune && m_config.pwrBandTxMemory ()) {
    m_pwrBandTxMemory[curBand] = a; // remember our Tx pwr
  }
  if (m_PwrBandSetOK && m_tune && m_config.pwrBandTuneMemory()) {
    m_pwrBandTuneMemory[curBand] = a; // remember our Tune pwr
  }
  Q_EMIT outAttenuationChanged (dBAttn);
}

void MainWindow::on_actionShort_list_of_add_on_prefixes_and_suffixes_triggered()
{
  if (!m_prefixes) {
    m_prefixes.reset (new HelpTextWindow {tr ("Prefixes")
                                            , R"(Type 1 Prefixes:

 1A    1S    3A    3B6   3B8   3B9   3C    3C0   3D2   3D2C  3D2R  3DA   3V    3W    3X   
 3Y    3YB   3YP   4J    4L    4S    4U1I  4U1U  4W    4X    5A    5B    5H    5N    5R   
 5T    5U    5V    5W    5X    5Z    6W    6Y    7O    7P    7Q    7X    8P    8Q    8R   
 9A    9G    9H    9J    9K    9L    9M2   9M6   9N    9Q    9U    9V    9X    9Y    A2   
 A3    A4    A5    A6    A7    A9    AP    BS7   BV    BV9   BY    C2    C3    C5    C6   
 C9    CE    CE0X  CE0Y  CE0Z  CE9   CM    CN    CP    CT    CT3   CU    CX    CY0   CY9  
 D2    D4    D6    DL    DU    E3    E4    EA    EA6   EA8   EA9   EI    EK    EL    EP   
 ER    ES    ET    EU    EX    EY    EZ    F     FG    FH    FJ    FK    FKC   FM    FO   
 FOA   FOC   FOM   FP    FR    FRG   FRJ   FRT   FT5W  FT5X  FT5Z  FW    FY    M     MD   
 MI    MJ    MM    MU    MW    H4    H40   HA    HB    HB0   HC    HC8   HH    HI    HK   
 HK0A  HK0M  HL    HM    HP    HR    HS    HV    HZ    I     IS    IS0   J2    J3    J5   
 J6    J7    J8    JA    JDM   JDO   JT    JW    JX    JY    K     KG4   KH0   KH1   KH2  
 KH3   KH4   KH5   KH5K  KH6   KH7   KH8   KH9   KL    KP1   KP2   KP4   KP5   LA    LU   
 LX    LY    LZ    OA    OD    OE    OH    OH0   OJ0   OK    OM    ON    OX    OY    OZ   
 P2    P4    PA    PJ2   PJ7   PY    PY0F  PT0S  PY0T  PZ    R1F   R1M   S0    S2    S5   
 S7    S9    SM    SP    ST    SU    SV    SVA   SV5   SV9   T2    T30   T31   T32   T33  
 T5    T7    T8    T9    TA    TF    TG    TI    TI9   TJ    TK    TL    TN    TR    TT   
 TU    TY    TZ    UA    UA2   UA9   UK    UN    UR    V2    V3    V4    V5    V6    V7   
 V8    VE    VK    VK0H  VK0M  VK9C  VK9L  VK9M  VK9N  VK9W  VK9X  VP2E  VP2M  VP2V  VP5  
 VP6   VP6D  VP8   VP8G  VP8H  VP8O  VP8S  VP9   VQ9   VR    VU    VU4   VU7   XE    XF4  
 XT    XU    XW    XX9   XZ    YA    YB    YI    YJ    YK    YL    YN    YO    YS    YU   
 YV    YV0   Z2    Z3    ZA    ZB    ZC4   ZD7   ZD8   ZD9   ZF    ZK1N  ZK1S  ZK2   ZK3  
 ZL    ZL7   ZL8   ZL9   ZP    ZS    ZS8   KC4   E5   

Type 1 Suffixes:    /0 /1 /2 /3 /4 /5 /6 /7 /8 /9 /A /P)", {"Courier", 10}});
  }
  m_prefixes->showNormal();
  m_prefixes->raise ();
}

bool MainWindow::shortList(QString callsign) const
{
  int n=callsign.length();
  int i1=callsign.indexOf("/");
  Q_ASSERT(i1>0 and i1<n);
  QString t1=callsign.mid(0,i1);
  QString t2=callsign.mid(i1+1,n-i1-1);
  bool b=(m_pfx.contains(t1) or m_sfx.contains(t2));
  return b;
}

void MainWindow::pskSetLocal ()
{
  if (!m_config.spot_to_psk_reporter ()) return;

  // find the station row, if any, that matches the band we are on
  auto stations = m_config.stations ();
  auto matches = stations->match (stations->index (0, StationList::band_column)
                                  , Qt::DisplayRole
                                  , ui->bandComboBox->currentText ()
                                  , 1
                                  , Qt::MatchExactly);
  QString antenna_description;
  if (!matches.isEmpty ()) {
    antenna_description = stations->index (matches.first ().row ()
                                           , StationList::description_column).data ().toString ();
  }
  // qDebug() << "To PSKreporter: local station details";
  m_psk_Reporter.setLocalStation(m_config.my_callsign (), m_config.my_grid (), antenna_description);
}

void MainWindow::transmitDisplay (bool transmitting)
{
  if (transmitting == m_transmitting) {
    if (transmitting) {
      ui->signal_meter_widget->setValue(0,0);
      if (m_monitoring) monitor (false);
      m_btxok=true;
    }

    auto QSY_allowed = !transmitting or m_config.tx_QSY_allowed () or
      !m_config.split_mode ();
    if (ui->cbHoldTxFreq->isChecked ()) {
      ui->TxFreqSpinBox->setEnabled (QSY_allowed);
      ui->pbT2R->setEnabled (QSY_allowed);
    }

    if (m_mode!="WSPR" and m_mode!="FST4W") {
      if(m_config.enable_VHF_features ()) {
        ui->TxFreqSpinBox->setEnabled (true);
      } else {
        ui->TxFreqSpinBox->setEnabled (QSY_allowed and !m_bFastMode);
        ui->pbR2T->setEnabled (QSY_allowed);
        ui->cbHoldTxFreq->setEnabled (QSY_allowed);
      }
    }

    // the following are always disallowed in transmit
    ui->menuMode->setEnabled (!transmitting);
  }
}

void MainWindow::on_sbFtol_valueChanged(int value)
{
  m_wideGraph->setTol (value);
  statusUpdate ();
}

void::MainWindow::VHF_features_enabled(bool b)
{
  if(m_mode!="JT4" and m_mode!="JT65" and m_mode!="Q65") b=false;
  if(b and m_mode!="Q65" and (ui->actionInclude_averaging->isChecked() or
             ui->actionInclude_correlation->isChecked())) {
    ui->actionDeepestDecode->setChecked (true);
  }
  ui->actionInclude_averaging->setVisible (b);
  ui->actionInclude_correlation->setVisible (b && m_mode!="Q65");
  ui->actionMessage_averaging->setEnabled(b && (m_mode=="JT4" or m_mode=="JT65"));
  ui->actionEnable_AP_JT65->setVisible (b && m_mode=="JT65");

  if(!b && m_msgAvgWidget and (SpecOp::FOX != m_config.special_op_id()) and !m_config.autoLog()) {
    if(m_msgAvgWidget->isVisible() and m_mode!="JT4" and m_mode!="JT9" and m_mode!="JT65") {
      m_msgAvgWidget->close();
    }
  }
}

void MainWindow::on_sbTR_valueChanged(int value)
{
  //  if(!m_bFastMode and n>m_nSubMode) m_MinW=m_nSubMode;
  if(m_bFastMode or m_mode=="FreqCal" or m_mode=="FST4" or m_mode=="FST4W" or m_mode=="Q65") {
    m_TRperiod = value;
    if (m_mode == "FST4" || m_mode == "FST4W" || m_mode=="Q65")
      {
        if (m_TRperiod < 60)
          {
            ui->lh_decodes_headings_label->setText("  UTC   dB   DT Freq    " + tr ("Message"));
            if (m_mode != "FST4W")
              {
                ui->rh_decodes_headings_label->setText("  UTC   dB   DT Freq    " + tr ("Message"));
              }
          }
        else
          {
            ui->lh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
            if (m_mode != "FST4W")
              {
                ui->rh_decodes_headings_label->setText("UTC   dB   DT Freq    " + tr ("Message"));
              }
          }
      
       if ("Q65" == m_mode)
         {
         switch (value)
             {
              case 15: ui->sbSubmode->setMaximum (2); break;
              case 30: ui->sbSubmode->setMaximum (3); break;
              default: ui->sbSubmode->setMaximum (4); break;
              }
          }
       } 
    m_fastGraph->setTRPeriod (value);
    m_modulator->setTRPeriod (value); // TODO - not thread safe
    m_detector->setTRPeriod (value);  // TODO - not thread safe
    m_wideGraph->setPeriod (value, m_nsps);
    progressBar.setMaximum (value);
  }
  if(m_mode=="FST4") chk_FST4_freq_range();
//  if(m_transmitting) on_stopTxButton_clicked();      //### Is this needed or desirable? ###
  on_sbSubmode_valueChanged(ui->sbSubmode->value());
  statusUpdate ();
}

void MainWindow::on_sbTR_FST4W_valueChanged(int value)
{
  on_sbTR_valueChanged(value);
}

QChar MainWindow::current_submode () const
{
  QChar submode {0};
  if (m_mode.contains (QRegularExpression {R"(^(JT65|JT9|JT4|Q65)$)"})
      && (m_config.enable_VHF_features () || "JT4" == m_mode))
    {
      submode = m_nSubMode + 65;
    }
  return submode;
}

void MainWindow::on_sbSubmode_valueChanged(int n)
{
  m_nSubMode=n;
  m_wideGraph->setSubMode(m_nSubMode);
  auto submode = current_submode ();
  if (submode != QChar::Null) {
    QString t{m_mode + " " + submode};
    if(m_mode=="Q65") t=m_mode + "-" + QString::number(m_TRperiod) + submode;
    mode_label.setText (t);
  } else {
    mode_label.setText (m_mode);
  }
  if(m_mode=="Q65") {
    if(((m_nSubMode==4 && m_TRperiod==60.0) || (m_nSubMode==3 && m_TRperiod==30.0) ||
       (m_nSubMode==2 && m_TRperiod==15.0)) && ui->TxFreqSpinBox->value()!=700) {
      ui->TxFreqSpinBox->setStyleSheet("QSpinBox{background-color:red}");
    } else {
      ui->TxFreqSpinBox->setStyleSheet("");
    }
  }
  if(m_mode=="JT9") {
    if(m_nSubMode<4) {
      ui->cbFast9->setChecked(false);
      on_cbFast9_clicked(false);
      ui->cbFast9->setEnabled(false);
      ui->sbTR->setVisible(false);
      m_TRperiod=60.0;
    } else {
      ui->cbFast9->setEnabled(true);
    }
    ui->sbTR->setVisible(m_bFast9);
    if(m_bFast9) ui->TxFreqSpinBox->setValue(700);
  }
  if(m_transmitting and m_bFast9 and m_nSubMode>=4) transmit (99.0);
  statusUpdate ();
}

void MainWindow::on_cbFast9_clicked(bool b)
{
  if(m_mode=="JT9") {
    m_bFast9=b;
//    ui->cbAutoSeq->setVisible(b);
    on_actionJT9_triggered();
  }

  if(b) {
    m_TRperiod = ui->sbTR->value ();
  } else {
    m_TRperiod=60.0;
  }
  progressBar.setMaximum(int(m_TRperiod));
  m_wideGraph->setPeriod(m_TRperiod,m_nsps);
  fast_config(b);
  statusChanged ();
}


void MainWindow::on_cbShMsgs_toggled(bool b)
{
  ui->cbTx6->setEnabled(b);
  m_bShMsgs=b;
  if(b) ui->cbSWL->setChecked(false);
  if(m_bShMsgs and (m_mode=="MSK144")) ui->rptSpinBox->setValue(1);
  int it0=itone[0];
  int ntx=m_ntx;
  m_lastCallsign.clear ();      // ensure Tx5 gets updated
  genStdMsgs(m_rpt);
  itone[0]=it0;
  if(ntx==1) ui->txrb1->setChecked(true);
  if(ntx==2) ui->txrb2->setChecked(true);
  if(ntx==3) ui->txrb3->setChecked(true);
  if(ntx==4) ui->txrb4->setChecked(true);
  if(ntx==5) ui->txrb5->setChecked(true);
  if(ntx==6) ui->txrb6->setChecked(true);
}

void MainWindow::on_cbSWL_toggled(bool b)
{
  if(b) ui->cbShMsgs->setChecked(false);
}

void MainWindow::on_cbTx6_toggled(bool)
{
  genCQMsg ();
}

// Takes a decoded CQ line and sets it up for reply
void MainWindow::replyToCQ (QTime time, qint32 snr, float delta_time, quint32 delta_frequency
                            , QString const& mode, QString const& message_text
                            , bool /*low_confidence*/, quint8 modifiers)
{
  QString format_string {"%1 %2 %3 %4 %5 %6"};
  auto const& time_string = time.toString ("~" == mode || "&" == mode || "+" == mode
                                           || (m_TRperiod < 60. && ("`" == mode || ":" == mode))
                                           ? "hhmmss" : "hhmm");
  auto text = message_text;
  auto ap_pos = text.lastIndexOf (QRegularExpression {R"((?:\?\s)?(?:a[0-9]|q[0-9][0-9]?)$)"});
  if (ap_pos >= 0)
    {
      // beware of decodes ending on shorter version of wanted call so
      // add a space
      text = text.left (ap_pos).trimmed () + ' ';
    }
  auto message_line = format_string
    .arg (time_string)
    .arg (snr, 3)
    .arg (delta_time, 4, 'f', 1)
    .arg (delta_frequency, 4)
    .arg (mode, -2)
    .arg (text);
  QTextCursor start {ui->decodedTextBrowser->document ()};
  start.movePosition (QTextCursor::End);
  auto cursor = ui->decodedTextBrowser->document ()->find (message_line, start, QTextDocument::FindBackward);
  if (cursor.isNull ())
    {
      // try again with with -0.0 delta time
      cursor = ui->decodedTextBrowser->document ()->find (format_string
                                                          .arg (time_string)
                                                          .arg (snr, 3)
                                                          .arg ('-' + QString::number (delta_time, 'f', 1), 4)
                                                          .arg (delta_frequency, 4)
                                                          .arg (mode, -2)
                                                          .arg (text), start, QTextDocument::FindBackward);
    }
  if (!cursor.isNull ())
    {
      if (m_config.udpWindowToFront ())
        {
          show ();
          raise ();
          activateWindow ();
        }
      if (m_config.udpWindowRestore () && isMinimized ())
        {
          showNormal ();
          raise ();
        }
      if (text.contains (QRegularExpression {R"(^(CQ |CQDX |QRZ ))"})) {
        // a message we are willing to accept and auto reply to
        m_bDoubleClicked = true;
      }
      DecodedText message {message_line};
      Qt::KeyboardModifiers kbmod {modifiers << 24};
      processMessage (message, kbmod);
      tx_watchdog (false);
      QApplication::alert (this);
    }
  else
    {
      qDebug () << "process reply message ignored, decode not found:" << message_line;
    }
}

void MainWindow::locationChange (QString const& location)
{
  QString grid {location.trimmed ()};
  int len;

  // string 6 chars or fewer, interpret as a grid, or use with a 'GRID:' prefix
  if (grid.size () > 6) {
    if (grid.toUpper ().startsWith ("GRID:")) {
      grid = grid.mid (5).trimmed ();
    }
    else {
      // TODO - support any other formats, e.g. latlong? Or have that conversion done external to wsjtx
      return;
    }
  }
  if (MaidenheadLocatorValidator::Acceptable == MaidenheadLocatorValidator ().validate (grid, len)) {
    qDebug() << "locationChange: Grid supplied is " << grid;
    if (m_config.my_grid () != grid) {
      m_config.set_location (grid);
      genStdMsgs (m_rpt, false);
      statusUpdate ();
    }
  } else {
    qDebug() << "locationChange: Invalid grid " << grid;
  }
}

void MainWindow::replayDecodes ()
{
  // we accept this request even if the setting to accept UDP requests
  // is not checked

  // attempt to parse the decoded text
  for (QTextBlock block = ui->decodedTextBrowser->document ()->firstBlock (); block.isValid (); block = block.next ())
    {
      auto message = block.text ();
      message = message.left (message.indexOf (QChar::Nbsp)); // discard
                                                              // any
                                                              // appended info
      if (message.size() >= 4 && message.left (4) != "----")
        {
          auto const& parts = message.split (' ', SkipEmptyParts);
          if (parts.size () >= 5 && parts[3].contains ('.')) {
            postWSPRDecode (false, parts);
          } else {
            postDecode (false, message);
          }
      }
  }
  statusChanged ();
}

void MainWindow::postDecode (bool is_new, QString const& message)
{
  auto const& decode = message.trimmed ();
  auto const& parts = decode.left (22).split (' ', SkipEmptyParts);
  if (parts.size () >= 5)
    {
      auto has_seconds = parts[0].size () > 4;
      m_messageClient->decode (is_new
                               , QTime::fromString (parts[0], has_seconds ? "hhmmss" : "hhmm")
                               , parts[1].toInt ()
                               , parts[2].toFloat (), parts[3].toUInt (), parts[4]
                               , decode.mid (has_seconds ? 24 : 22)
                               , QChar {'?'} == decode.mid (has_seconds ? 24 + 36 : 22 + 36, 1)
                               , m_diskData);
    }
}

void MainWindow::postWSPRDecode (bool is_new, QStringList parts)
{
  if (parts.size () < 8)
    {
      parts.insert (6, "");
    }
  m_messageClient->WSPR_decode (is_new, QTime::fromString (parts[0], "hhmm"), parts[1].toInt ()
                                , parts[2].toFloat (), Radio::frequency (parts[3].toFloat (), 6)
                                , parts[4].toInt (), parts[5], parts[6], parts[7].toInt ()
                                , m_diskData);
}

void MainWindow::networkError (QString const& e)
{
  if (m_splash && m_splash->isVisible ()) m_splash->hide ();
  if (MessageBox::Retry == MessageBox::warning_message (this, tr ("Network Error")
                                                        , tr ("Error: %1\nUDP server %2:%3")
                                                        .arg (e)
                                                        .arg (m_config.udp_server_name ())
                                                        .arg (m_config.udp_server_port ())
                                                        , QString {}
                                                        , MessageBox::Cancel | MessageBox::Retry
                                                        , MessageBox::Cancel))
    {
      // retry server lookup
      m_messageClient->set_server (m_config.udp_server_name (), m_config.udp_interface_names ());
    }
}

void MainWindow::on_syncSpinBox_valueChanged(int n)
{
  m_minSync=n;
}

void MainWindow::p1ReadFromStdout()                        //p1readFromStdout
{
  QString t1;
  while(p1.canReadLine()) {
    QString t(p1.readLine());
    if(ui->cbNoOwnCall->isChecked()) {
      if(t.contains(" " + m_config.my_callsign() + " ")) continue;
      if(t.contains(" <" + m_config.my_callsign() + "> ")) continue;
    }
    if(t.indexOf("<DecodeFinished>") >= 0) {
      m_bDecoded = m_nWSPRdecodes > 0;
      if(!m_diskData) {
        WSPR_history(m_dialFreqRxWSPR, m_nWSPRdecodes);
        if(m_nWSPRdecodes==0 and ui->band_hopping_group_box->isChecked()) {
          t = " " + tr ("Receiving") + " " + m_mode + " ----------------------- " +
              m_config.bands ()->find (m_dialFreqRxWSPR);
          t=beacon_start_time (-m_TRperiod / 2) + ' ' + t.rightJustified (66, '-');
          ui->decodedTextBrowser->appendText(t);
        }
        killFileTimer.start (45*1000); //Kill in 45s (for slow modes)
      }
      m_nWSPRdecodes=0;
      ui->DecodeButton->setChecked (false);
      if (m_uploadWSPRSpots
          && m_config.is_transceiver_online ()) { // need working rig control
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        uploadTimer.start(QRandomGenerator::global ()->bounded (0, 20000)); // Upload delay
#else
        uploadTimer.start(20000 * qrand()/((double)RAND_MAX + 1.0)); // Upload delay
#endif
      } else {
        QFile f {QDir::toNativeSeparators (m_config.writeable_data_dir ().absoluteFilePath ("wspr_spots.txt"))};
        if (f.exists ()) f.remove ();
      }
      m_RxLog=0;
      m_startAnother=m_loopall;
      m_decoderBusy = false;
      statusUpdate ();
    } else {

      int n=t.length();
      t=t.mid(0,n-2) + "                                                  ";
      t.remove(QRegExp("\\s+$"));
      QStringList rxFields = t.split(QRegExp("\\s+"));
      QString rxLine;
      QString grid="";
      if ( rxFields.count() == 8 ) {
          rxLine = QString("%1 %2 %3 %4 %5   %6  %7  %8")
                  .arg(rxFields.at(0), 4)
                  .arg(rxFields.at(1), 4)
                  .arg(rxFields.at(2), 5)
                  .arg(rxFields.at(3), 11)
                  .arg(rxFields.at(4), 4)
                  .arg(rxFields.at(5).leftJustified (12))
                  .arg(rxFields.at(6), -6)
                  .arg(rxFields.at(7), 3);
          postWSPRDecode (true, rxFields);
          grid = rxFields.at(6);
      } else if ( rxFields.count() == 7 ) { // Type 2 message
          rxLine = QString("%1 %2 %3 %4 %5   %6  %7  %8")
                  .arg(rxFields.at(0), 4)
                  .arg(rxFields.at(1), 4)
                  .arg(rxFields.at(2), 5)
                  .arg(rxFields.at(3), 11)
                  .arg(rxFields.at(4), 4)
                  .arg(rxFields.at(5).leftJustified (12))
                  .arg("", -6)
                  .arg(rxFields.at(6), 3);
          postWSPRDecode (true, rxFields);
      } else {
          rxLine = t;
      }
      if(grid!="") {
        double utch=0.0;
        int nAz,nEl,nDmiles,nDkm,nHotAz,nHotABetter;
        azdist_(const_cast <char *> ((m_config.my_grid () + "      ").left (6).toLatin1 ().constData ()),
                const_cast <char *> ((grid + "      ").left (6).toLatin1 ().constData ()),&utch,
                &nAz,&nEl,&nDmiles,&nDkm,&nHotAz,&nHotABetter,6,6);
        QString t1;
        if(m_config.miles()) {
          t1 = t1.asprintf("%7d",nDmiles);
        } else {
          t1 = t1.asprintf("%7d",nDkm);
        }
        rxLine += t1;
      }

      if (rxLine.left (4) != m_tBlankLine) {
        ui->decodedTextBrowser->new_period ();
        if (m_config.insert_blank ()) {
          QString band;
          Frequency f=1000000.0*rxFields.at(3).toDouble()+0.5;
          band = ' ' + m_config.bands ()->find (f);
          ui->decodedTextBrowser->appendText(band.rightJustified (71, '-'));
        }
        m_tBlankLine = rxLine.left(4);
      }
      m_nWSPRdecodes += 1;
      ui->decodedTextBrowser->appendText(rxLine);
    }
  }
}

QString MainWindow::beacon_start_time (int n)
{
  auto bt = qt_truncate_date_time_to (QDateTime::currentDateTimeUtc ().addSecs (n), m_TRperiod * 1.e3);
  if (m_TRperiod < 60.)
    {
      return bt.toString ("HHmmss");
    }
  else
    {
      return bt.toString ("HHmm");
    }
}

void MainWindow::WSPR_history(Frequency dialFreq, int ndecodes)
{
  QDateTime t=QDateTime::currentDateTimeUtc().addSecs(-60);
  QString t1=t.toString("yyMMdd");
  QString t2=beacon_start_time (-m_TRperiod / 2);
  QString t3;
  t3 = t3.asprintf("%13.6f",0.000001*dialFreq);
  if(ndecodes<0) {
    t1=t1 + " " + t2 + t3 + "  T";
  } else {
    QString t4;
    t4 = t4.asprintf("%4d",ndecodes);
    t1=t1 + " " + t2 + t3 + "  R" + t4;
  }
  QFile f {m_config.writeable_data_dir ().absoluteFilePath ("WSPR_history.txt")};
  if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
    QTextStream out(&f);
    out << t1
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        << Qt::endl
#else
        << endl
#endif
      ;
    f.close();
  } else {
    MessageBox::warning_message (this, tr ("File Error")
                                 , tr ("Cannot open \"%1\" for append: %2")
                                 .arg (f.fileName ()).arg (f.errorString ()));
  }
}

void MainWindow::uploadWSPRSpots (bool direct_post, QString const& decode_text)
{
  // do not spot if disabled, replays, or if rig control not working
  if(!m_uploadWSPRSpots || m_diskData || !m_config.is_transceiver_online ()) return;
  if(m_uploading && !decode_text.size ()) {
    qDebug() << "Previous upload has not completed, spots were lost";
    wsprNet->abortOutstandingRequests ();
    m_uploading = false;
  }
  QString rfreq = QString("%1").arg((m_dialFreqRxWSPR + m_wideGraph->rxFreq ()) / 1e6, 0, 'f', 6);
  QString tfreq = QString("%1").arg((m_dialFreqRxWSPR +
                        ui->TxFreqSpinBox->value()) / 1e6, 0, 'f', 6);
  auto pct = QString::number (ui->autoButton->isChecked () ? ui->sbTxPercent->value () : 0);
  if (direct_post)
    {
      // queues one FST4W spot
      wsprNet->post (m_config.my_callsign (), m_config.my_grid (), rfreq, tfreq,
                     m_mode, m_TRperiod, pct,
                     QString::number (m_dBm), version (), decode_text);
    }
  else
    {
      // queues spots for each decode in wspr_spots.txt
      wsprNet->upload (m_config.my_callsign (), m_config.my_grid (), rfreq, tfreq,
                       m_mode, m_TRperiod, pct,
                       QString::number (m_dBm), version (),
                       m_config.writeable_data_dir ().absoluteFilePath ("wspr_spots.txt"));
    }
  // trigger upload of any queued spots
  if (!decode_text.size ())
    {
      m_uploading = true;
    }
}

void MainWindow::uploadResponse(QString const& response)
{
  if (response == "done") {
    m_uploading=false;
  } else {
    if (response.startsWith ("Upload Failed")) {
      m_uploading=false;
    }
    qDebug () << "WSPRnet.org status:" << response;
  }
}

void MainWindow::on_TxPowerComboBox_currentIndexChanged(int index)
{
  m_dBm = ui->TxPowerComboBox->itemData (index).toInt ();
}

void MainWindow::on_cbUploadWSPR_Spots_toggled(bool b)
{
  m_uploadWSPRSpots=b;
}

void MainWindow::on_WSPRfreqSpinBox_valueChanged(int n)
{
  ui->TxFreqSpinBox->setValue(n);
}

void MainWindow::on_sbFST4W_RxFreq_valueChanged(int n)
{
  m_wideGraph->setRxFreq(n);
  statusUpdate ();
}

void MainWindow::on_sbFST4W_FTol_valueChanged(int n)
{
  ui->sbFST4W_RxFreq->setSingleStep(n);
  m_wideGraph->setTol(n);
  statusUpdate ();
}

void MainWindow::on_pbTxNext_clicked(bool b)
{
  if (b && !ui->autoButton->isChecked ())
    {
      ui->autoButton->click (); // make sure Tx is possible
    }
}

void MainWindow::WSPR_scheduling ()
{
  if (ui->pbTxNext->isEnabled () && ui->pbTxNext->isChecked ())
    {
      // Tx Next button overrides all scheduling
      m_WSPR_tx_next = true;
      return;
    }
  QString t=ui->RoundRobin->currentText();
  if(m_mode=="FST4W" and t != tr ("Random")) {
    bool ok;
    int i=t.left (1).toInt (&ok) - 1;
    if (!ok) return;
    int n=t.right (1).toInt (&ok);
    if (!ok || 0 == n) return;

    qint64 ms = QDateTime::currentMSecsSinceEpoch() % 86400000;
    int nsec=ms/1000;
    int ntr=m_TRperiod;
    int j=((nsec+ntr-1) % (n*ntr))/ntr;
    m_WSPR_tx_next = i == j;
    return;
  }
  m_WSPR_tx_next = false;
  if (!ui->sbTxPercent->isEnabled ())
    {
      return;                   // don't schedule if %age disabled
    }
  if (m_config.is_transceiver_online () // need working rig control for hopping
      && !m_config.is_dummy_rig ()
      && ui->band_hopping_group_box->isChecked ()) {
    auto hop_data = m_WSPR_band_hopping.next_hop (m_auto);
    qDebug () << "hop data: period:" << hop_data.period_name_
              << "frequencies index:" << hop_data.frequencies_index_
              << "tune:" << hop_data.tune_required_
              << "tx:" << hop_data.tx_next_;
    m_WSPR_tx_next = hop_data.tx_next_;
    if (hop_data.frequencies_index_ >= 0) { // new band
      ui->bandComboBox->setCurrentIndex (hop_data.frequencies_index_);
      on_bandComboBox_activated (hop_data.frequencies_index_);
      // Execute user's hardware controller
      auto band = m_config.bands ()->find (m_freqNominal).remove ('m');
#if defined(Q_OS_WIN)
      // On  windows   we  use  CMD.EXE   to  find  and   execute  the
      // user_hardware executable. This means  that the first matching
      // file extension  on the PATHEXT environment  variable found on
      // the PATH  environment variable  path list. This  give maximum
      // flexibility  for  users  to   write  user_hardware  in  their
      // language of choice,  and place the file anywhere  on the PATH
      // environment  variable.  Equivalent  to  typing  user_hardware
      // without any path or extension at the CMD.EXE prompt.
      p3.start("CMD", QStringList {"/C", "user_hardware", band});
#else
      // On non-Windows systems we expect the user_hardware executable
      // to be anywhere in the paths specified in the PATH environment
      // variable  path list,  and  executable.  Equivalent to  typing
      // user_hardware without any path at the shell prompt.
      p3.start("/bin/sh", QStringList {"-c", "user_hardware " + band});
#endif

      // Produce a short tuneup signal
      m_tuneup = false;
      if (hop_data.tune_required_) {
        m_tuneup = true;
        on_tuneButton_clicked (true);
        tuneATU_Timer.start (2500);
      }
    }

    // Display grayline status
    band_hopping_label.setText (hop_data.period_name_);
  }
  else {
    m_WSPR_tx_next = m_WSPR_band_hopping.next_is_tx(m_mode=="FST4W");
  }
}

void MainWindow::astroUpdate ()
{
  if (m_astroWidget)
    {
      // no Doppler correction while CTRL pressed allows manual tuning
      if (Qt::ControlModifier & QApplication::queryKeyboardModifiers ()) return;

      auto correction = m_astroWidget->astroUpdate(QDateTime::currentDateTimeUtc (),
                                                   m_config.my_grid(), m_hisGrid,
                                                   m_freqNominal,
                                                   "Echo" == m_mode, m_transmitting,
                                                   !m_config.tx_QSY_allowed (), m_TRperiod);
      // no Doppler correction in Tx if rig can't do it
      if (m_transmitting && !m_config.tx_QSY_allowed ()) return;
      if (!m_astroWidget->doppler_tracking ()) return;
      if ((m_monitoring || m_transmitting)
          // no Doppler correction below 6m
          && m_freqNominal >= 50000000
          && m_config.split_mode ())
        {
          // adjust for rig resolution
          if (m_config.transceiver_resolution () > 2)
            {
              correction.rx = (correction.rx + 50) / 100 * 100;
              correction.tx = (correction.tx + 50) / 100 * 100;
            }
          else if (m_config.transceiver_resolution () > 1)
            {
              correction.rx = (correction.rx + 10) / 20 * 20;
              correction.tx = (correction.tx + 10) / 20 * 20;
            }
          else if (m_config.transceiver_resolution () > 0)
            {
              correction.rx = (correction.rx + 5) / 10 * 10;
              correction.tx = (correction.tx + 5) / 10 * 10;
            }
          else if (m_config.transceiver_resolution () < -2)
            {
              correction.rx = correction.rx / 100 * 100;
              correction.tx = correction.tx / 100 * 100;
            }
          else if (m_config.transceiver_resolution () < -1)
            {
              correction.rx = correction.rx / 20 * 20;
              correction.tx = correction.tx / 20 * 20;
            }
          else if (m_config.transceiver_resolution () < 0)
            {
              correction.rx = correction.rx / 10 * 10;
              correction.tx = correction.tx / 10 * 10;
            }
          m_astroCorrection = correction;
          if (m_reverse_Doppler)
            {
              m_astroCorrection.reverse ();
            }
        }
      else
        {
          m_astroCorrection = {};
        }

      setRig ();
    }
}

void MainWindow::setRig (Frequency f)
{
  if (f)
    {
      m_freqNominal = f;
      genCQMsg ();
      m_freqTxNominal = m_freqNominal;
      if (m_astroWidget) m_astroWidget->nominal_frequency (m_freqNominal, m_freqTxNominal);
    }
  if (m_mode == "FreqCal"
      && m_frequency_list_fcal_iter != m_config.frequencies ()->end ()) {
    m_freqNominal = m_frequency_list_fcal_iter->frequency_ - ui->RxFreqSpinBox->value ();
  }
  if(m_transmitting && !m_config.tx_QSY_allowed ()) return;
  if ((m_monitoring || m_transmitting) && m_config.transceiver_online ())
    {
      if (m_transmitting && m_config.split_mode ())
        {
          m_config.transceiver_tx_frequency (m_freqTxNominal + m_astroCorrection.tx);
        }
      else
        {
          m_config.transceiver_frequency (m_freqNominal + m_astroCorrection.rx);
        }
    }
}

void MainWindow::fastPick(int x0, int x1, int y)
{
  float pixPerSecond=12000.0/512.0;
  if(m_TRperiod<30.0) pixPerSecond=12000.0/256.0;
  if(m_mode!="MSK144") return;
  if(!m_decoderBusy) {
    dec_data.params.newdat=0;
    dec_data.params.nagain=1;
    m_nPick=1;
    if(y > 120) m_nPick=2;
    m_t0Pick=x0/pixPerSecond;
    m_t1Pick=x1/pixPerSecond;
    m_dataAvailable=true;
    decode();
  }
}

void MainWindow::on_actionMeasure_reference_spectrum_triggered()
{
  if(!m_monitoring) on_monitorButton_clicked (true);
  m_bRefSpec=true;
}

void MainWindow::on_actionMeasure_phase_response_triggered()
{
  if(m_bTrain) { 
    m_bTrain=false;
    MessageBox::information_message (this, tr ("Phase Training Disabled"));
  } else {
    m_bTrain=true;
    MessageBox::information_message (this, tr ("Phase Training Enabled"));
  }
}

void MainWindow::on_actionErase_reference_spectrum_triggered()
{
  m_bClearRefSpec=true;
}

void MainWindow::freqCalStep()
{
  if (m_frequency_list_fcal_iter == m_config.frequencies ()->end ()
      || ++m_frequency_list_fcal_iter == m_config.frequencies ()->end ()) {
    m_frequency_list_fcal_iter = m_config.frequencies ()->begin ();
  }

  // allow for empty list
  if (m_frequency_list_fcal_iter != m_config.frequencies ()->end ()) {
    setRig (m_frequency_list_fcal_iter->frequency_ - ui->RxFreqSpinBox->value ());
  }
}

void MainWindow::on_sbCQTxFreq_valueChanged(int)
{
  setXIT (ui->TxFreqSpinBox->value ());
}

void MainWindow::on_cbCQTx_toggled(bool b)
{
  ui->sbCQTxFreq->setEnabled(b);
  genCQMsg();
  if(b) {
    ui->txrb6->setChecked(true);
    m_ntx=6;
    m_QSOProgress = CALLING;
  }
  setRig ();
  setXIT (ui->TxFreqSpinBox->value ());
}

void MainWindow::statusUpdate () const
{
  if (!ui || m_block_udp_status_updates) return;
  auto submode = current_submode ();
  auto ftol = ui->sbFtol->value ();
  if ("FST4W" == m_mode)
    {
      ftol = ui->sbFST4W_FTol->value ();
    }
  else if (!(ui->sbFtol->isVisible () && ui->sbFtol->isEnabled ()))
    {
      ftol = quint32_max;
    }
  auto tr_period = ui->sbTR->value ();
  auto rx_frequency = ui->RxFreqSpinBox->value ();
  if ("FST4W" == m_mode)
    {
      tr_period = ui->sbTR_FST4W->value ();
      rx_frequency = ui->sbFST4W_RxFreq->value ();
    }
  else if (!(ui->sbTR->isVisible () && ui->sbTR->isEnabled ()))
    {
      tr_period = quint32_max;
    }
  m_messageClient->status_update (m_freqNominal, m_mode, m_hisCall,
                                  QString::number (ui->rptSpinBox->value ()),
                                  m_mode, ui->autoButton->isChecked (),
                                  m_transmitting, m_decoderBusy,
                                  rx_frequency, ui->TxFreqSpinBox->value (),
                                  m_config.my_callsign (), m_config.my_grid (),
                                  m_hisGrid, m_tx_watchdog,
                                  submode != QChar::Null ? QString {submode} : QString {}, m_bFastMode,
                                  static_cast<quint8> (m_config.special_op_id ()),
                                  ftol, tr_period, m_multi_settings->configuration_name (),
                                  m_currentMessage);
}

void MainWindow::childEvent (QChildEvent * e)
{
  if (e->child ()->isWidgetType ())
    {
      switch (e->type ())
        {
        case QEvent::ChildAdded: add_child_to_event_filter (e->child ()); break;
        case QEvent::ChildRemoved: remove_child_from_event_filter (e->child ()); break;
        default: break;
        }
    }
  QMainWindow::childEvent (e);
}

// add widget and any child widgets to our event filter so that we can
// take action on key press ad mouse press events anywhere in the main window
void MainWindow::add_child_to_event_filter (QObject * target)
{
  if (target && target->isWidgetType ())
    {
      target->installEventFilter (this);
    }
  auto const& children = target->children ();
  for (auto iter = children.begin (); iter != children.end (); ++iter)
    {
      add_child_to_event_filter (*iter);
    }
}

// recursively remove widget and any child widgets from our event filter
void MainWindow::remove_child_from_event_filter (QObject * target)
{
  auto const& children = target->children ();
  for (auto iter = children.begin (); iter != children.end (); ++iter)
    {
      remove_child_from_event_filter (*iter);
    }
  if (target && target->isWidgetType ())
    {
      target->removeEventFilter (this);
    }
}

void MainWindow::tx_watchdog (bool triggered)
{
  auto prior = m_tx_watchdog;
  m_tx_watchdog = triggered;
  if (triggered)
    {
      m_bTxTime=false;
      if (m_tune) stop_tuning ();
      if (m_auto) auto_tx_mode (false);
      tx_status_label.setStyleSheet ("QLabel{color: #000000; background-color: #ff0000}");
      tx_status_label.setText (tr ("Runaway Tx watchdog"));
      QApplication::alert (this);
    }
  else
    {
      m_idleMinutes = 0;
      update_watchdog_label ();
    }
  if (prior != triggered) statusUpdate ();
}

void MainWindow::update_watchdog_label ()
{
  if (m_config.watchdog () && m_mode!="WSPR" && m_mode!="FST4W")
    {
      watchdog_label.setText (tr ("WD:%1m").arg (m_config.watchdog () - m_idleMinutes));
      watchdog_label.setVisible (true);
    }
  else
    {
      watchdog_label.setText (QString {});
      watchdog_label.setVisible (false);
    }
}

void MainWindow::on_cbMenus_toggled(bool b)
{
  select_geometry (!b ? 2 : ui->actionSWL_Mode->isChecked () ? 1 : 0);
}

void MainWindow::on_cbCQonly_toggled(bool)
{  //Fix this -- no decode here?
  to_jt9(m_ihsym,1,-1);                //Send m_ihsym to jt9[.exe] and start decoding
  decodeBusy(true);
}

void MainWindow::on_cbFirst_toggled(bool b)
{
  if (b) {
    if (m_auto && CALLING == m_QSOProgress) {
      ui->cbFirst->setStyleSheet ("QCheckBox{color:red}");
    }
  } else {
    ui->cbFirst->setStyleSheet ("");
  }
}

void MainWindow::on_cbAutoSeq_toggled(bool b)
{
  if(!b) ui->cbFirst->setChecked(false);
  ui->cbFirst->setVisible((m_mode=="FT8" or m_mode=="FT4" or m_mode=="FST4"
                           or m_mode=="Q65") and b);
}

void MainWindow::on_measure_check_box_stateChanged (int state)
{
  m_config.enable_calibration (Qt::Checked != state);
}

void MainWindow::write_transmit_entry (QString const& file_name)
{
  QFile f {m_config.writeable_data_dir ().absoluteFilePath (file_name)};
  if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
      QTextStream out(&f);
      auto time = QDateTime::currentDateTimeUtc ();
      time = time.addSecs (-fmod(double(time.time().second()),m_TRperiod));
      out << time.toString("yyMMdd_hhmmss")
          << "  Transmitting " << qSetRealNumberPrecision (12) << (m_freqNominal / 1.e6)
          << " MHz  " << m_mode
          << ":  " << m_currentMessage
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
          << Qt::endl
#else
          << endl
#endif
        ;
      f.close();
    }
  else
    {
      auto const& message = tr ("Cannot open \"%1\" for append: %2")
        .arg (f.fileName ()).arg (f.errorString ());
      QTimer::singleShot (0, [=] {                   // don't block guiUpdate
          MessageBox::warning_message (this, tr ("Log File Error"), message);
        });
    }
}

// -------------------------- Code for FT8 DXpedition Mode ---------------------------

void MainWindow::hound_reply ()
{
  if (!m_tune) {
    //Select TX3, set TxFreq to FoxFreq, and Force Auto ON.
    ui->txrb3->setChecked (true);
    m_nSentFoxRrpt = 1;
    ui->rptSpinBox->setValue(m_rptSent.toInt());
    if (!m_auto) auto_tx_mode(true);
    ui->TxFreqSpinBox->setValue (m_nFoxFreq);
  }
}

void MainWindow::on_sbNlist_valueChanged(int n)
{
  m_Nlist=n;
}

void MainWindow::on_sbNslots_valueChanged(int n)
{
  m_Nslots=n;
  if(m_config.special_op_id()!=SpecOp::FOX) return;
  QString t;
  t = t.asprintf(" NSlots %d",m_Nslots);
  writeFoxQSO(t);
}

void MainWindow::on_sbMax_dB_valueChanged(int n)
{
  m_max_dB=n;
  if(m_config.special_op_id()!=SpecOp::FOX) return;
  QString t;
  t = t.asprintf(" Max_dB %d",m_max_dB);
  writeFoxQSO(t);
}

void MainWindow::on_pbFoxReset_clicked()
{
  if(m_config.special_op_id()!=SpecOp::FOX) return;
  auto button = MessageBox::query_message (this, tr ("Confirm Reset"),
      tr ("Are you sure you want to clear the QSO queues?"));
  if(button == MessageBox::Yes) {
    QFile f(m_config.temp_dir().absoluteFilePath("houndcallers.txt"));
    f.remove();
    ui->decodedTextBrowser->setText("");
    ui->textBrowser4->setText("");
    m_houndQueue.clear();
    m_foxQSO.clear();
    m_foxQSOinProgress.clear();
    writeFoxQSO(" Reset");
  }
}

void MainWindow::on_comboBoxHoundSort_activated(int index)
{
  if(index!=-99) houndCallers();            //Silence compiler warning
}

//------------------------------------------------------------------------------
QString MainWindow::sortHoundCalls(QString t, int isort, int max_dB)
{
/* Called from "houndCallers()" to sort the list of calling stations by
 * specified criteria.
 *
 * QString "t" contains a list of Hound callers read from file "houndcallers.txt".
 *    isort=0: random    (shuffled order)
 *          1: Call
 *          2: Grid
 *          3: SNR       (reverse order)
 *          4: Distance  (reverse order)
*/

  QMap<QString,QString> map;
  QStringList lines,lines2;
  QString msg,houndCall,t1;
  QString ABC{"ABCDEFGHIJKLMNOPQRSTUVWXYZ _"};
  QList<int> list;
  int i,j,k,n,nlines;
  bool bReverse=(isort >= 3);

  isort=qAbs(isort);
// Save only the most recent transmission from each caller.
  lines = t.split("\n");
  nlines=lines.length()-1;
  for(i=0; i<nlines; i++) {
    msg=lines.at(i);                        //key = callsign
    if(msg.mid(13,1)==" ") msg=msg.mid(0,13) + "...." + msg.mid(17);
    houndCall=msg.split(" ").at(0);         //value = "call grid snr freq dist age"
    map[houndCall]=msg;
  }

  j=0;
  t="";
  for(auto a: map.keys()) {
    t1=map[a].split(" ",SkipEmptyParts).at(2);
    int nsnr=t1.toInt();                         // get snr
    if(nsnr <= max_dB) {                         // keep only if snr in specified range
      if(isort==1) t += map[a] + "\n";
      if(isort==3 or isort==4) {
        i=2;                                           // sort Hound calls by snr
        if(isort==4) i=4;                              // sort Hound calls by distance
        t1=map[a].split(" ",SkipEmptyParts).at(i);
        n=1000*(t1.toInt()+100) + j;                   // pack (snr or dist) and index j into n
        list.insert(j,n);                              // add n to list at [j]
      }

      if(isort==2) {                                   // sort Hound calls by grid
        t1=map[a].split(" ",SkipEmptyParts).at(1);
        if(t1=="....") t1="ZZ99";
        int i1=ABC.indexOf(t1.mid(0,1));
        int i2=ABC.indexOf(t1.mid(1,1));
        n=100*(26*i1+i2)+t1.mid(2,2).toInt();
        n=1000*n + j;                                 // pack ngrid and index j into n
        list.insert(j,n);                             // add n to list at [j]
      }

      lines2.insert(j,map[a]);                        // add map[a] to lines2 at [j]
      j++;
    }
  }

  if(isort>1) {
    if(bReverse) {
      std::sort (list.begin (), list.end (), std::greater<int> ());
    } else {
      std::sort (list.begin (), list.end ());
    }
  }

  if(isort>1) {
    for(i=0; i<j; i++) {
      k=list[i]%1000;
      n=list[i]/1000 - 100;
      t += lines2.at(k) + "\n";
    }
  }

  int nn=lines2.length();
  if(isort==0) {                                      // shuffle Hound calls to random order
    int a[nn];
    for(i=0; i<nn; i++) {
      a[i]=i;
    }
    for(i=nn-1; i>-1; i--) {
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
      j = (i + 1) * QRandomGenerator::global ()->generateDouble ();
#else
      j=(i+1)*double(qrand())/RAND_MAX;
#endif
      std::swap (a[j], a[i]);
      t += lines2.at(a[i]) + "\n";
    }
  }

  int i0=t.indexOf("\n") + 1;
  m_nSortedHounds=0;
  if(i0 > 0) {
    m_nSortedHounds=qMin(t.length(),m_Nlist*i0)/i0; // Number of sorted & displayed Hounds
  }
  m_houndCallers=t.mid(0,m_Nlist*i0);

  return m_houndCallers;
}

//------------------------------------------------------------------------------
void MainWindow::selectHound(QString line)
{
/* Called from doubleClickOnCall() in DXpedition Fox mode.
 * QString "line" is a user-selected line from left text window.
 * The line may be selected by double-clicking; alternatively, hitting
 * <Enter> is equivalent to double-clicking on the top-most line.
*/

  if(line.length()==0) return;
  QString houndCall=line.split(" ",SkipEmptyParts).at(0);

// Don't add a call already enqueued or in QSO
  if(ui->textBrowser4->toPlainText().indexOf(houndCall) >= 0) return;

  QString houndGrid=line.split(" ",SkipEmptyParts).at(1);  // Hound caller's grid
  QString rpt=line.split(" ",SkipEmptyParts).at(2);        // Hound SNR

  m_houndCallers=m_houndCallers.remove(line+"\n");      // Remove t from sorted Hound list
  m_nSortedHounds--;
  ui->decodedTextBrowser->setText(m_houndCallers);   // Populate left window with Hound callers
  QString t1=houndCall + "          ";
  QString t2=rpt;
  if(rpt.mid(0,1) != "-" and rpt.mid(0,1) != "+") t2="+" + rpt;
  if(t2.length()==2) t2=t2.mid(0,1) + "0" + t2.mid(1,1);
  t1=t1.mid(0,12) + t2;
  ui->textBrowser4->displayFoxToBeCalled(t1); // Add hound call and rpt to tb4
  t1=t1 + " " + houndGrid;                    // Append the grid
  m_houndQueue.enqueue(t1);     // Put this hound into the queue
  writeFoxQSO(" Sel:  " + t1);
  QTextCursor cursor = ui->textBrowser4->textCursor();
  cursor.setPosition(0);                                 // Scroll to top of list
  ui->textBrowser4->setTextCursor(cursor);
}

//------------------------------------------------------------------------------
void MainWindow::houndCallers()
{
/* Called from decodeDone(), in DXpedition Fox mode.  Reads decodes from file
 * "houndcallers.txt", ignoring any that are not addressed to MyCall, are already
 * in the stack, or with whom a QSO has been started.  Others are considered to
 * be Hounds eager for a QSO.  We add caller information (Call, Grid, SNR, Freq,
 * Distance, Age, and Continent) to a list, sort the list by specified criteria,
 * and display the top N_Hounds entries in the left text window.
*/
  QFile f(m_config.temp_dir().absoluteFilePath("houndcallers.txt"));
  if(f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream s(&f);
    QString t="";
    QString line,houndCall,paddedHoundCall;
    m_nHoundsCalling=0;
    int nTotal=0;  //Total number of decoded Hounds calling Fox in 4 most recent Rx sequences

// Read and process the file of Hound callers.
    while(!s.atEnd()) {
      line=s.readLine();
      nTotal++;
      int i0=line.indexOf(" ");
      houndCall=line.mid(0,i0);
      paddedHoundCall=houndCall + " ";
      //Don't list a hound already in the queue
      if(!ui->textBrowser4->toPlainText().contains(paddedHoundCall)) {
        if(m_loggedByFox[houndCall].contains(m_lastBand))   continue;   //already logged on this band
        if(m_foxQSO.contains(houndCall)) continue;   //still in the QSO map
        auto const& entity = m_logBook.countries ()->lookup (houndCall);
        auto const& continent = AD1CCty::continent (entity.continent);

//If we are using a directed CQ, ignore Hound calls that do not comply.
        QString CQtext=ui->comboBoxCQ->currentText();
        if(CQtext.length()==5 and (continent!=CQtext.mid(3,2))) continue;
        int nCallArea=-1;
        if(CQtext.length()==4) {
          for(int i=houndCall.length()-1; i>0; i--) {
            if(houndCall.mid(i,1).toInt() > 0) nCallArea=houndCall.mid(i,1).toInt();
            if(houndCall.mid(i,1)=="0") nCallArea=0;
            if(nCallArea>=0) break;
          }
          if(nCallArea!=CQtext.mid(3,1).toInt()) continue;
        }
//This houndCall passes all tests, add it to the list.
        t = t + line + "  " + continent + "\n";
        m_nHoundsCalling++;                // Number of accepted Hounds to be sorted
      }
    }
    if(m_foxLogWindow) m_foxLogWindow->callers (nTotal);

// Sort and display accumulated list of Hound callers
    if(t.length()>30) {
      m_isort=ui->comboBoxHoundSort->currentIndex();
      QString t1=sortHoundCalls(t,m_isort,m_max_dB);
      ui->decodedTextBrowser->setText(t1);
    }
    f.close();
  }
}

void MainWindow::foxRxSequencer(QString msg, QString houndCall, QString rptRcvd)
{
/* Called from "readFromStdOut()" to process decoded messages of the form
 * "myCall houndCall R+rpt".
 *
 * If houndCall matches a callsign in one of our active QSO slots, we
 * prepare to send "houndCall RR73" to that caller.
*/
  if(m_foxQSO.contains(houndCall)) {
    m_foxQSO[houndCall].rcvd=rptRcvd.mid(1);  //Save report Rcvd, for the log
    m_foxQSO[houndCall].tFoxRrpt=m_tFoxTx;    //Save time R+rpt was received
    writeFoxQSO(" Rx:   " + msg.trimmed());
  } else {
    for(QString hc: m_foxQSO.keys()) {        //Check for a matching compound call
      if(hc.contains("/"+houndCall) or hc.contains(houndCall+"/")) {
        m_foxQSO[hc].rcvd=rptRcvd.mid(1);  //Save report Rcvd, for the log
        m_foxQSO[hc].tFoxRrpt=m_tFoxTx;    //Save time R+rpt was received
        writeFoxQSO(" Rx:   " + msg.trimmed());
      }
    }
  }
}

void MainWindow::foxTxSequencer()
{
/* Called from guiUpdate at the point where an FT8 Fox-mode transmission
 * is to be started.
 *
 * Determine what the Tx message(s) will be for each active slot, call
 * foxgen() to generate and accumulate the corresponding waveform.
*/

  qint64 now=QDateTime::currentMSecsSinceEpoch()/1000;
  QStringList list1;                        //Up to NSlots Hound calls to be sent RR73
  QStringList list2;                        //Up to NSlots Hound calls to be sent a report
  QString fm;                               //Fox message to be transmitted
  QString hc,hc1,hc2;                       //Hound calls
  QString t,rpt;
  qint32  islot=0;
  qint32  n1,n2,n3;

  m_tFoxTx++;                               //Increment Fox Tx cycle counter

  //Is it time for a stand-alone CQ?
  if(m_tFoxTxSinceCQ >= m_foxCQtime and ui->cbMoreCQs->isChecked()) {
    fm=ui->comboBoxCQ->currentText() + " " + m_config.my_callsign();
    if(!fm.contains("/")) {
      //If Fox is not a compound callsign, add grid to the CQ message.
      fm += " " + m_config.my_grid().mid(0,4);
      m_fullFoxCallTime=now;
    }
    m_tFoxTx0=m_tFoxTx;                     //Remember when we sent a CQ
    islot++;
    foxGenWaveform(islot-1,fm);
    goto Transmit;
  }
//Compile list1: up to NSLots Hound calls to be sent RR73
  for(QString hc: m_foxQSO.keys()) {           //Check all Hound calls: First priority
    if(m_foxQSO[hc].tFoxRrpt<0) continue;
    if(m_foxQSO[hc].tFoxRrpt - m_foxQSO[hc].tFoxTxRR73 > 3) {
      //Has been a long time since we sent RR73
      list1 << hc;                          //Add to list1
      m_foxQSO[hc].tFoxTxRR73 = m_tFoxTx;   //Time RR73 is sent
      m_foxQSO[hc].nRR73++;                 //Increment RR73 counter
      if(list1.size()==m_Nslots) goto list1Done;
    }
  }

  for(QString hc: m_foxQSO.keys()) {           //Check all Hound calls: Second priority
    if(m_foxQSO[hc].tFoxRrpt<0) continue;
    if(m_foxQSO[hc].tFoxTxRR73 < 0) {
      //Have not yet sent RR73
      list1 << hc;                          //Add to list1
      m_foxQSO[hc].tFoxTxRR73 = m_tFoxTx;   //Time RR73 is sent
      m_foxQSO[hc].nRR73++;                 //Increment RR73 counter
      if(list1.size()==m_Nslots) goto list1Done;
    }
  }

  for(QString hc: m_foxQSO.keys()) {           //Check all Hound calls: Third priority
    if(m_foxQSO[hc].tFoxRrpt<0) continue;
    if(m_foxQSO[hc].tFoxTxRR73 <= m_foxQSO[hc].tFoxRrpt) {
      //We received R+rpt more recently than we sent RR73
      list1 << hc;                          //Add to list1
      m_foxQSO[hc].tFoxTxRR73 = m_tFoxTx;   //Time RR73 is sent
      m_foxQSO[hc].nRR73++;                 //Increment RR73 counter
      if(list1.size()==m_Nslots) goto list1Done;
    }
  }

list1Done:
//Compile list2: Up to Nslots Hound calls to be sent a report.
  for(int i=0; i<m_foxQSOinProgress.count(); i++) {
    //First do those for QSOs in progress
    hc=m_foxQSOinProgress.at(i);
    if((m_foxQSO[hc].tFoxRrpt < 0) and (m_foxQSO[hc].ncall < m_maxStrikes)) {
      //Sent him a report and have not received R+rpt: call him again
      list2 << hc;                          //Add to list2
      if(list2.size()==m_Nslots) goto list2Done;
    }
  }

  while(!m_houndQueue.isEmpty()) {
    //Start QSO with a new Hound
    t=m_houndQueue.dequeue();             //Fetch new hound from queue
    int i0=t.indexOf(" ");
    hc=t.mid(0,i0);                       //hound call
    list2 << hc;                          //Add new Hound to list2
    m_foxQSOinProgress.enqueue(hc);       //Put him in the QSO queue
    m_foxQSO[hc].grid=t.mid(16,4);        //Hound grid
    rpt=t.mid(12,3);                      //report to send Hound
    m_foxQSO[hc].sent=rpt;                //Report to send him
    m_foxQSO[hc].ncall=0;                 //Start a new Hound
    m_foxQSO[hc].nRR73 = 0;               //Have not sent RR73
    m_foxQSO[hc].rcvd = -99;              //Have not received R+rpt
    m_foxQSO[hc].tFoxRrpt = -1;           //Have not received R+rpt
    m_foxQSO[hc].tFoxTxRR73 = -1;         //Have not sent RR73
    rm_tb4(hc);                           //Remove this Hound from tb4
    if(list2.size()==m_Nslots) goto list2Done;
    if(m_foxQSO.count()>=2*m_Nslots) goto list2Done;
  }

list2Done:
  n1=list1.size();
  n2=list2.size();
  n3=qMax(n1,n2);
  if(n3>m_Nslots) n3=m_Nslots;
  for(int i=0; i<n3; i++) {
    hc1="";
    fm="";
    if(i<n1 and i<n2) {
      hc1=list1.at(i);
      hc2=list2.at(i);
      m_foxQSO[hc2].ncall++;
      fm = Radio::base_callsign(hc1) + " RR73; " + Radio::base_callsign(hc2) +
          " <" + m_config.my_callsign() + "> " + m_foxQSO[hc2].sent;
    }
    if(i<n1 and i>=n2) {
      hc1=list1.at(i);
      fm = Radio::base_callsign(hc1) + " " + m_baseCall + " RR73";                 //Standard FT8 message
    }

    if(hc1!="") {
      // Log this QSO!
      auto QSO_time = QDateTime::currentDateTimeUtc ();
      m_hisCall=hc1;
      m_hisGrid=m_foxQSO[hc1].grid;
      m_rptSent=m_foxQSO[hc1].sent;
      m_rptRcvd=m_foxQSO[hc1].rcvd;
      if (!m_foxLogWindow) on_fox_log_action_triggered ();
      if (m_logBook.fox_log ()->add_QSO (QSO_time, m_hisCall, m_hisGrid, m_rptSent, m_rptRcvd, m_lastBand))
        {
          writeFoxQSO (QString {" Log:  %1 %2 %3 %4 %5"}.arg (m_hisCall).arg (m_hisGrid)
                       .arg (m_rptSent).arg (m_rptRcvd).arg (m_lastBand));
          on_logQSOButton_clicked ();
          m_foxRateQueue.enqueue (now); //Add present time in seconds
                                        //to Rate queue.
        }
      m_loggedByFox[hc1] += (m_lastBand + " ");
    }

    if(i<n2 and fm=="") {
      hc2=list2.at(i);
      m_foxQSO[hc2].ncall++;
      fm = Radio::base_callsign(hc2) + " " + m_baseCall + " " + m_foxQSO[hc2].sent; //Standard FT8 message
    }
    islot++;
    foxGenWaveform(islot-1,fm);                             //Generate tx waveform
  }

  if(islot < m_Nslots) {
    //At least one slot is still open
    if(islot==0 or ((m_tFoxTx-m_tFoxTx0>=4) and ui->cbMoreCQs->isChecked())) {
      //Roughly every 4th Tx sequence, put a CQ message in an otherwise empty slot
      fm=ui->comboBoxCQ->currentText() + " " + m_config.my_callsign();
      if(!fm.contains("/")) {
        fm += " " + m_config.my_grid().mid(0,4);
        m_tFoxTx0=m_tFoxTx;                                //Remember when we send a CQ
        m_fullFoxCallTime=now;
      }
      islot++;
      foxGenWaveform(islot-1,fm);
    }
  }

Transmit:
  foxcom_.nslots=islot;
  foxcom_.nfreq=ui->TxFreqSpinBox->value();
  if(m_config.split_mode()) foxcom_.nfreq = foxcom_.nfreq - m_XIT;  //Fox Tx freq
  QString foxCall=m_config.my_callsign() + "         ";
  ::memcpy(foxcom_.mycall, foxCall.toLatin1(),sizeof foxcom_.mycall);   //Copy Fox callsign into foxcom_
  foxgen_();
  m_tFoxTxSinceCQ++;

  for(QString hc: m_foxQSO.keys()) {               //Check for strikeout or timeout
    if(m_foxQSO[hc].ncall>=m_maxStrikes) m_foxQSO[hc].ncall++;
    bool b1=((m_tFoxTx - m_foxQSO[hc].tFoxRrpt) > 2*m_maxFoxWait) and
        (m_foxQSO[hc].tFoxRrpt > 0);
    bool b2=((m_tFoxTx - m_foxQSO[hc].tFoxTxRR73) > m_maxFoxWait) and
        (m_foxQSO[hc].tFoxTxRR73>0);
    bool b3=(m_foxQSO[hc].ncall >= m_maxStrikes+m_maxFoxWait);
    bool b4=(m_foxQSO[hc].nRR73 >= m_maxStrikes);
    if(b1 or b2 or b3 or b4) {
      m_foxQSO.remove(hc);
      m_foxQSOinProgress.removeOne(hc);
    }
  }

  while(!m_foxRateQueue.isEmpty()) {
    qint64 age = now - m_foxRateQueue.head();
    if(age < 3600) break;
    m_foxRateQueue.dequeue();
  }
  if (m_foxLogWindow)
    {
      m_foxLogWindow->rate (m_foxRateQueue.size ());
      m_foxLogWindow->queued (m_foxQSOinProgress.count ());
    }
}

void MainWindow::rm_tb4(QString houndCall)
{
  if(houndCall=="") return;
  QString t="";
  QString tb4=ui->textBrowser4->toPlainText();
  QStringList list=tb4.split("\n");
  int n=list.size();
  int j=0;
  for (int i=0; i<n; i++) {
    if(j>0) t += "\n";
    QString line=list.at(i);
    if(!line.contains(houndCall + " ")) {
      j++;
      t += line;
    }
  }
  t.replace("\n\n","\n");
  ui->textBrowser4->setText(t);
}

void MainWindow::doubleClickOnFoxQueue(Qt::KeyboardModifiers modifiers)
{
  if(modifiers==9999) return;                               //Silence compiler warning
  QTextCursor cursor=ui->textBrowser4->textCursor();
  cursor.setPosition(cursor.selectionStart());
  QString houndCall=cursor.block().text().mid(0,12).trimmed();
  rm_tb4(houndCall);
  writeFoxQSO(" Del:  " + houndCall);
  QQueue<QString> tmpQueue;
  while(!m_houndQueue.isEmpty()) {
    QString t=m_houndQueue.dequeue();
    QString hc=t.mid(0,12).trimmed();
    if(hc != houndCall) tmpQueue.enqueue(t);
  }
  m_houndQueue.swap(tmpQueue);
}

void MainWindow::foxGenWaveform(int i,QString fm)
{
//Generate and accumulate the Tx waveform
  fm += "                                        ";
  fm=fm.mid(0,40);
  if(fm.mid(0,3)=="CQ ") m_tFoxTxSinceCQ=-1;

  QString txModeArg;
  txModeArg = txModeArg.asprintf("FT8fox %d",i+1);
  ui->decodedTextBrowser2->displayTransmittedText(fm.trimmed(), txModeArg,
        ui->TxFreqSpinBox->value()+60*i,m_bFastMode,m_TRperiod);
  foxcom_.i3bit[i]=0;
  if(fm.indexOf("<")>0) foxcom_.i3bit[i]=1;
  strncpy(&foxcom_.cmsg[i][0],fm.toLatin1(),40);   //Copy this message into cmsg[i]
  if(i==0) m_fm1=fm;
  QString t;
  t = t.asprintf(" Tx%d:  ",i+1);
  writeFoxQSO(t + fm.trimmed());
}

void MainWindow::writeFoxQSO(QString const& msg)
{
  QString t;
  t = t.asprintf("%3d%3d%3d",m_houndQueue.count(),m_foxQSOinProgress.count(),m_foxQSO.count());
  QFile f {m_config.writeable_data_dir ().absoluteFilePath ("FoxQSO.txt")};
  if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
    QTextStream out(&f);
    out << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") << "  "
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        << Qt::fixed
#else
        << fixed
#endif
        << qSetRealNumberPrecision (3) << (m_freqNominal/1.e6)
        << t << msg
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        << Qt::endl
#else
        << endl
#endif
      ;
    f.close();
  } else {
    MessageBox::warning_message (this, tr("File Open Error"),
      tr("Cannot open \"%1\" for append: %2").arg(f.fileName()).arg(f.errorString()));
  }
}

/*################################################################################### */
void MainWindow::foxTest()
{
  QFile f("steps.txt");
  if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;

  QFile fdiag("diag.txt");
  if(!fdiag.open(QIODevice::WriteOnly | QIODevice::Text)) return;

  QTextStream s(&f);
  QTextStream sdiag(&fdiag);
  QString line;
  QString t;
  QString msg;
  QString hc1;
  QString rptRcvd;
  qint32 n=0;

  while(!s.atEnd()) {
    line=s.readLine();
    if(line.length()==0) continue;
    if(line.mid(0,4).toInt()==0) line="                                     " + line;
    if(line.contains("NSlots")) {
      n=line.mid(44,1).toInt();
      ui->sbNslots->setValue(n);
    }
    if(line.contains("Sel:")) {
      t=line.mid(43,6) + "       " + line.mid(54,4) + "   " + line.mid(50,3);
      selectHound(t);
    }

    if(line.contains("Del:")) {
      int i0=line.indexOf("Del:");
      hc1=line.mid(i0+6);
      int i1=hc1.indexOf(" ");
      hc1=hc1.mid(0,i1);
      rm_tb4(hc1);
      writeFoxQSO(" Del:  " + hc1);
      QQueue<QString> tmpQueue;
      while(!m_houndQueue.isEmpty()) {
        t=m_houndQueue.dequeue();
        QString hc=t.mid(0,6).trimmed();
        if(hc != hc1) tmpQueue.enqueue(t);
      }
      m_houndQueue.swap(tmpQueue);
    }
    if(line.contains("Rx:"))  {
      msg=line.mid(43);
      t=msg.mid(24);
      int i0=t.indexOf(" ");
      hc1=t.mid(i0+1);
      int i1=hc1.indexOf(" ");
      hc1=hc1.mid(0,i1);
      int i2=qMax(msg.indexOf("R+"),msg.indexOf("R-"));
      if(i2>20) {
        rptRcvd=msg.mid(i2,4);
        foxRxSequencer(msg,hc1,rptRcvd);
      }
    }
    if(line.contains("Tx1:")) {
      foxTxSequencer();
    } else {
      t = t.asprintf("%3d %3d %3d %3d %5d   ",m_houndQueue.count(),
                m_foxQSOinProgress.count(),m_foxQSO.count(),
                m_loggedByFox.count(),m_tFoxTx);
      sdiag << t << line.mid(37).trimmed() << "\n";
    }
  }
}

void MainWindow::write_all(QString txRx, QString message)
{
  QString line;
  QString t;
  QString msg;
  QString mode_string;

  if (message.size () > 5 && message[4]==' ') {
     msg=message.mid(4,-1);
  } else {
     msg=message.mid(6,-1);
  }

  if (message.size () > 19 && message[19]=='#') {
     mode_string="JT65  ";
  } else if (message.size () > 19 && message[19]=='@') {
     mode_string="JT9   ";
  } else if(m_mode=="Q65") {
    mode_string=mode_label.text();
  } else {
     mode_string=m_mode.leftJustified(6,' ');
  }

  msg=msg.mid(0,15) + msg.mid(18,-1);

  t = t.asprintf("%5d",ui->TxFreqSpinBox->value());
  if (txRx=="Tx") msg="   0  0.0" + t + " " + message;
  auto time = QDateTime::currentDateTimeUtc ();
  if( txRx=="Rx" && !m_bFastMode ) time=m_dateTimeSeqStart;

  t = t.asprintf("%10.3f ",m_freqNominal/1.e6);
  if (m_diskData) {
    if (m_fileDateTime.size()==11) {
      line=m_fileDateTime + "  " + t + txRx + " " + mode_string + msg;
    } else {
      line=m_fileDateTime + t + txRx + " " + mode_string + msg;
    } 
  } else {
    line=time.toString("yyMMdd_hhmmss") + t + txRx + " " + mode_string + msg;
  }

  QString file_name="ALL.TXT";
  if (m_mode=="WSPR") file_name="ALL_WSPR.TXT";
  QFile f{m_config.writeable_data_dir().absoluteFilePath(file_name)};
  if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
    QTextStream out(&f);
    out << line.trimmed()
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 0)
        << Qt::endl
#else
        << endl
#endif
      ;
    f.close();
  } else {
    auto const& message2 = tr ("Cannot open \"%1\" for append: %2")
        .arg (f.fileName ()).arg (f.errorString ());
    QTimer::singleShot (0, [=] {                   // don't block guiUpdate
      MessageBox::warning_message(this, tr ("Log File Error"), message2); });
  }
}

void MainWindow::chkFT4()
{
  if(m_mode!="FT4") return;
  ui->cbAutoSeq->setEnabled(true);
  ui->cbFirst->setVisible(true);
  ui->cbFirst->setEnabled(true);
  ui->labDXped->setVisible(m_config.special_op_id()!=SpecOp::NONE);
  ui->cbFirst->setVisible(ui->cbAutoSeq->isChecked());

  if (SpecOp::NONE < m_config.special_op_id () && SpecOp::FOX > m_config.special_op_id ()) {
    QString t0="";
    if(SpecOp::NA_VHF==m_config.special_op_id()) t0+="NA VHF";
    if(SpecOp::EU_VHF==m_config.special_op_id()) t0+="EU VHF";
    if(SpecOp::FIELD_DAY==m_config.special_op_id()) t0+="Field Day";
    if(SpecOp::RTTY==m_config.special_op_id()) t0+="RTTY";
    if(SpecOp::WW_DIGI==m_config.special_op_id()) t0+="WW_DIGI";
    if(t0=="") {
      ui->labDXped->setVisible(false);
    } else {
      ui->labDXped->setVisible(true);
      ui->labDXped->setText(t0);
    }
    on_contest_log_action_triggered();
  }
  if (SpecOp::HOUND == m_config.special_op_id() or SpecOp::FOX == m_config.special_op_id()) {
    ui->labDXped->setVisible(false);
  }

}

void MainWindow::on_pbBestSP_clicked()
{
  m_bBestSPArmed = !m_bBestSPArmed;
  if(m_bBestSPArmed and !m_transmitting) ui->pbBestSP->setStyleSheet ("QPushButton{color:red}");
  if(!m_bBestSPArmed) ui->pbBestSP->setStyleSheet ("");
  if(m_bBestSPArmed) m_dateTimeBestSP=QDateTime::currentDateTimeUtc();
}

void MainWindow::set_mode (QString const& mode)
{
    if ("FT4" == mode) on_actionFT4_triggered ();
    else if ("FST4" == mode) on_actionFST4_triggered ();
    else if ("FST4W" == mode) on_actionFST4W_triggered ();
    else if ("FT8" == mode) on_actionFT8_triggered ();
    else if ("JT4" == mode) on_actionJT4_triggered ();
    else if ("JT9" == mode) on_actionJT9_triggered ();
    else if ("JT65" == mode) on_actionJT65_triggered ();
    else if ("Q65" == mode) on_actionQ65_triggered ();
    else if ("FreqCal" == mode) on_actionFreqCal_triggered ();
    else if ("MSK144" == mode) on_actionMSK144_triggered ();
    else if ("WSPR" == mode) on_actionWSPR_triggered ();
    else if ("Echo" == mode) on_actionEcho_triggered ();
}

void MainWindow::remote_configure (QString const& mode, quint32 frequency_tolerance
                                   , QString const& submode, bool fast_mode, quint32 tr_period, quint32 rx_df
                                   , QString const& dx_call, QString const& dx_grid, bool generate_messages)
{
  if (mode.size ())
    {
      set_mode (mode);
    }
  auto is_FST4W = "FST4W" == m_mode;
  if (frequency_tolerance != quint32_max && (ui->sbFtol->isVisible () || is_FST4W))
    {
      m_block_udp_status_updates = true;
      if (is_FST4W)
        {
          ui->sbFST4W_FTol->setValue (frequency_tolerance);
        }
      else
        {
          ui->sbFtol->setValue (frequency_tolerance);
        }
      m_block_udp_status_updates = false;
    }
  if (submode.size () && ui->sbSubmode->isVisible ())
    {
      ui->sbSubmode->setValue (submode.toUpper ().at (0).toLatin1 () - 'A');
    }
  if (ui->cbFast9->isVisible () && ui->cbFast9->isChecked () != fast_mode)
    {
      ui->cbFast9->click ();
    }
  if (tr_period != quint32_max && ui->sbTR->isVisible ())
    {
      if (is_FST4W)
        {
          ui->sbTR_FST4W->setValue (tr_period);
          ui->sbTR_FST4W->interpretText ();
        }
      else
        {
          ui->sbTR->setValue (tr_period);
          ui->sbTR->interpretText ();
        }
    }
  if (rx_df != quint32_max && ui->RxFreqSpinBox->isVisible ())
    {
      m_block_udp_status_updates = true;
      if (is_FST4W)
        {
          ui->sbFST4W_RxFreq->setValue (rx_df);
          ui->sbFST4W_RxFreq->interpretText ();
        }
      else
        {
          ui->RxFreqSpinBox->setValue (rx_df);
          ui->RxFreqSpinBox->interpretText ();
        }
      m_block_udp_status_updates = false;
    }
  if (dx_call.size () && ui->dxCallEntry->isVisible ())
    {
      ui->dxCallEntry->setText (dx_call);
    }
  if (dx_grid.size () && ui->dxGridEntry->isVisible ())
    {
      ui->dxGridEntry->setText (dx_grid);
    }
  if (generate_messages && ui->genStdMsgsPushButton->isVisible ())
    {
      ui->genStdMsgsPushButton->click ();
    }
  if (m_config.udpWindowToFront ())
    {
      show ();
      raise ();
      activateWindow ();
    }
  if (m_config.udpWindowRestore () && isMinimized ())
    {
      showNormal ();
      raise ();
    }
  tx_watchdog (false);
  QApplication::alert (this);
}

QString MainWindow::WSPR_message()
{
  QString sdBm,msg0,msg1,msg2;
  sdBm = sdBm.asprintf(" %d",m_dBm);
  m_tx=1-m_tx;
  int i2=m_config.my_callsign().indexOf("/");
  if(i2>0
     || (6 == m_config.my_grid ().size ()
         && !ui->WSPR_prefer_type_1_check_box->isChecked ())) {
    if(i2<0) {                                                 // "Type 2" WSPR message
      msg1=m_config.my_callsign() + " " + m_config.my_grid().mid(0,4) + sdBm;
    } else {
      msg1=m_config.my_callsign() + sdBm;
    }
    msg0="<" + m_config.my_callsign() + "> " + m_config.my_grid();
    if(m_mode=="WSPR") msg0 += sdBm;
    if(m_tx==0) msg2=msg0;
    if(m_tx==1) msg2=msg1;
  } else {
    msg2=m_config.my_callsign() + " " + m_config.my_grid().mid(0,4) + sdBm; // Normal WSPR message
  }
  return msg2;
}
