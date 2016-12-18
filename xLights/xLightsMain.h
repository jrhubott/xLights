/***************************************************************
 * Name:      xLightsMain.h
 * Purpose:   Defines Application Frame
 * Author:    Matt Brown (dowdybrown@yahoo.com)
 * Created:   2012-11-03
 * Copyright: Matt Brown ()
 * License:
 **************************************************************/

#ifndef XLIGHTSMAIN_H
#define XLIGHTSMAIN_H

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

//(*Headers(xLightsFrame)
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/splitter.h>
#include <wx/spinctrl.h>
#include <wx/aui/aui.h>
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
#include <wx/gbsizer.h>
#include <wx/button.h>
#include <wx/dirdlg.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/gauge.h>
//*)

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <wx/fontdlg.h>
#include <wx/dir.h>
#include <unordered_map> //-DJ

#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#inlcude <windows.h>
#endif

#include <map>
#include <set>
#include <vector>

#include "EffectTreeDialog.h"
#include "xlights_out.h"
#include "PlayerFrame.h"
#include "EffectsPanel.h"
#include "AddShowDialog.h"
#include "PixelBuffer.h"
#include "NetInfo.h"
#include "ModelPreview.h"
#include "EffectAssist.h"
#include "SequenceData.h"
#include "UtilClasses.h"
#include "PhonemeDictionary.h"

#include "sequencer/EffectsGrid.h"
#include "sequencer/MainSequencer.h"
#include "sequencer/Waveform.h"
#include "TopEffectsPanel.h"
#include "TimingPanel.h"
#include "ColorPanel.h"
#include "PerspectivesPanel.h"
#include "EffectIconPanel.h"
#include "DisplayElementsPanel.h"
#include "effects/EffectManager.h"
#include "models/ModelManager.h"
#include "LayoutGroup.h"
#include "xLightsTimer.h"
#include "JobPool.h"
#include <log4cpp/Category.hh>

class EffectTreeDialog;
class ConvertDialog;
class ConvertLogDialog;
class wxDebugReport;

// max number of most recently used show directories on the File menu
#define MRU_LENGTH 4

// notebook pages
#define SETUPTAB 0
#define PREVIEWTAB 1
#define SCHEDULETAB 2
#define NEWSEQUENCER 3

#define PLAY_TYPE_STOPPED 0
#define PLAY_TYPE_EFFECT 1
#define PLAY_TYPE_MODEL  2
#define PLAY_TYPE_EFFECT_PAUSED 3
#define PLAY_TYPE_MODEL_PAUSED  4

#define FixedPages 4

#define TEXT_ENTRY_TIMING           0
#define TEXT_ENTRY_EFFECT           1
#define TEXT_ENTRY_DIALOG           2

class RenderCommandEvent;
class SelectedEffectChangedEvent;

wxDECLARE_EVENT(EVT_ZOOM, wxCommandEvent);
wxDECLARE_EVENT(EVT_GSCROLL, wxCommandEvent);
wxDECLARE_EVENT(EVT_TIME_SELECTED, wxCommandEvent);
wxDECLARE_EVENT(EVT_MOUSE_POSITION, wxCommandEvent);
wxDECLARE_EVENT(EVT_ROW_HEADINGS_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_WINDOW_RESIZED, wxCommandEvent);
wxDECLARE_EVENT(EVT_SELECTED_ROW_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_EFFECT_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_UNSELECTED_EFFECT, wxCommandEvent);
wxDECLARE_EVENT(EVT_EFFECT_DROPPED, wxCommandEvent);
wxDECLARE_EVENT(EVT_PLAY_MODEL_EFFECT, wxCommandEvent);
wxDECLARE_EVENT(EVT_UPDATE_EFFECT, wxCommandEvent);
wxDECLARE_EVENT(EVT_FORCE_SEQUENCER_REFRESH, wxCommandEvent);
wxDECLARE_EVENT(EVT_LOAD_PERSPECTIVE, wxCommandEvent);
wxDECLARE_EVENT(EVT_SAVE_PERSPECTIVES, wxCommandEvent);
wxDECLARE_EVENT(EVT_PERSPECTIVES_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_EXPORT_MODEL, wxCommandEvent);
wxDECLARE_EVENT(EVT_PLAY_MODEL, wxCommandEvent);
wxDECLARE_EVENT(EVT_COPY_MODEL_EFFECTS, wxCommandEvent);
wxDECLARE_EVENT(EVT_PASTE_MODEL_EFFECTS, wxCommandEvent);
wxDECLARE_EVENT(EVT_MODEL_SELECTED, wxCommandEvent);
wxDECLARE_EVENT(EVT_PLAY_SEQUENCE, wxCommandEvent);
wxDECLARE_EVENT(EVT_TOGGLE_PLAY, wxCommandEvent);
wxDECLARE_EVENT(EVT_PAUSE_SEQUENCE, wxCommandEvent);
wxDECLARE_EVENT(EVT_STOP_SEQUENCE, wxCommandEvent);
wxDECLARE_EVENT(EVT_SEQUENCE_FIRST_FRAME, wxCommandEvent);
wxDECLARE_EVENT(EVT_SEQUENCE_LAST_FRAME, wxCommandEvent);
wxDECLARE_EVENT(EVT_SEQUENCE_REPLAY_SECTION, wxCommandEvent);
wxDECLARE_EVENT(EVT_SHOW_DISPLAY_ELEMENTS, wxCommandEvent);
wxDECLARE_EVENT(EVT_IMPORT_TIMING, wxCommandEvent);
wxDECLARE_EVENT(EVT_IMPORT_NOTES, wxCommandEvent);
wxDECLARE_EVENT(EVT_CONVERT_DATA_TO_EFFECTS, wxCommandEvent);
wxDECLARE_EVENT(EVT_PROMOTE_EFFECTS, wxCommandEvent);
wxDECLARE_EVENT(EVT_RGBEFFECTS_CHANGED, wxCommandEvent);

static const wxString xlights_base_name       = "xLights";
static const wxString xlights_qualifier       = "Jason"; // " BETA";
static const wxString xlights_version_string  = "2016.54";
static const wxString xlights_build_date      = "Dec 14, 2016";

static const wxString strSupportedFileTypes = "LOR Music Sequences (*.lms)|*.lms|LOR Animation Sequences (*.las)|*.las|HLS hlsIdata Sequences(*.hlsIdata)|*.hlsIdata|Vixen Sequences (*.vix)|*.vix|Glediator Record File (*.gled)|*.gled)|Lynx Conductor Sequences (*.seq)|*.seq|xLights Sequences(*.xseq)|*.xseq|xLights Imports(*.iseq)|*.iseq|Falcon Pi Player Sequences (*.fseq)|*.fseq";
static const wxString strSequenceSaveAsFileTypes = "xLights Sequences(*.xml)|*.xml";
static wxCriticalSection gs_xoutCriticalSection;



typedef SequenceData SeqDataType;

enum play_modes
{
    play_off,
    play_single,
    play_list,
    play_sched
};

enum SeqPlayerStates
{
    NO_SEQ,
    STARTING_MEDIA,
    STARTING_SEQ,
    PLAYING_MEDIA,
    PLAYING_SEQ,
    STARTING_SEQ_ANIM,
    PLAYING_SEQ_ANIM,
    PAUSE_SEQ,
    PAUSE_SEQ_ANIM,
    DELAY_AFTER_PLAY,
    PLAYING_EFFECT
};

class RenderEvent;
class wxDebugReportCompress;
class BufferPanel;
class LayoutPanel;
class RenderProgressDialog;
class RenderProgressInfo;

class xlAuiToolBar : public wxAuiToolBar {
public:
    xlAuiToolBar(wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxAUI_TB_DEFAULT_STYLE) :
    wxAuiToolBar(parent, id, pos, size, style) {}
    virtual ~xlAuiToolBar() {}

    wxSize &GetAbsoluteMinSize() {return m_absoluteMinSize;}
    wxSize GetMinSize() const {return m_absoluteMinSize;}
};

class FPSEvent
{
public:
    wxDateTime when;
    int frame;

    FPSEvent(int f)
    {
        frame = f;
        when = wxDateTime::UNow();
    }
};

class SchedTreeData : public wxTreeItemData
{
protected:
    wxString eventString;

public:
    SchedTreeData(const wxString& EventData = wxT("")) {eventString = EventData;};
    wxString GetString() const {return eventString;};
};

#ifdef __WXOSX__
#define DOUBLE_BUFFER(a)
#else
#define DOUBLE_BUFFER(a) a->SetDoubleBuffered(true);
#endif

class wxAuiToolBar;
class AUIToolbarButtonWrapper {
public:
    AUIToolbarButtonWrapper(wxAuiToolBar *tb, int i) : toolbar(tb), id(i) {}

    bool IsChecked();
    void SetValue(bool b);
    void Enable(bool b);
    void SetBitmap(const wxBitmap &bmp);
private:
    wxAuiToolBar *toolbar;
    int id;
};

class xLightsFrame: public wxFrame
{
public:

    xLightsFrame(wxWindow* parent,wxWindowID id = -1);
    virtual ~xLightsFrame();

    enum PlayListIds
    {
        CHKBOX_AUDIO,
        CHKBOX_VIDEO,
        CHKBOX_XLIGHTS,
        CHKBOX_MOVIEMODE,
        UP_BUTTON,
        DOWN_BUTTON,
        INFO_BUTTON,
        PLAY_BUTTON,
        PLAY_ALL_BUTTON,
        DELAY_BUTTON,
        CREATE_SCRIPT,
        PLAYLIST_LISTBOX,
        PLAYLIST_FILES,
        PLAYLIST_ADD,
        PLAYLIST_ADD_ALL,
        PLAYLIST_DELETE,
        PLAYLIST_DELETE_ALL,
        SCRIPT_HELP,
        PLAYLIST_LOGIC,
        REMOVE_SCRIPT_BUTTON,

        PLAY_LIST_MAX
    };
    long SecondsRemaining, EndTimeSec;
    int TxOverflowCnt, TxOverflowTotal;
    xOutput* xout;
    std::mutex saveLock;

    PhonemeDictionary dictionary;

    void BasicPrompt(char* prompt, char* buff, int size);
    void BasicOutput(char *msg);
    void BasicError(const char *msg);
    char ExtType(const wxString& ext);
    bool Play(wxString& filename, long delay);
    void StopPlayback();
    void StartScript(const char *scriptname);
    void EndScript(const char *scriptname);
    int  FindNotebookPage(wxString& pagename);
    wxWindow* FindNotebookControl(int nbidx, PlayListIds id);
    void SetEffectControls(const std::string &modelName, const std::string &name,
                           const SettingsMap &settings, const SettingsMap &palette,
                           bool setDefaults);
    void SetEffectControls(const SettingsMap &settings);
	void ApplySetting(wxString name, wxString value);
    void LoadPerspectivesMenu(wxXmlNode* perspectivesNode);
    struct PerspectiveId {
        int id;
        wxXmlNode* p;
    };

    PerspectiveId perspectives[10];
    void OnMenuItemLoadPerspectiveSelected(wxCommandEvent& event);
	bool SaveEffectsFile(bool backup = false);
    void MarkEffectsFileDirty();
    void CheckUnsavedChanges();
    void SetStatusText(const wxString &msg, int section = 0);
	std::string GetChannelToControllerMapping(long channel);
    void GetControllerDetailsForChannel(long channel, std::string& type, std::string& description, int& channeloffset, std::string &ip, std::string& u, std::string& inactive, int& output);

    enum LAYER_OPTIONS_e
    {
        EFFECT1,
        EFFECT2,
        EFF1MASK,
        EFF2MASK,
        EFF1UNMASK,
        EFF2UNMASK,
        EFFLAYERED,
        EFFAVERAGED,
        LASTLAYER
    };

    enum EFFECT_ASSIST_MODES
    {
        EFFECT_ASSIST_ALWAYS_ON,
        EFFECT_ASSIST_ALWAYS_OFF,
        EFFECT_ASSIST_TOGGLE_MODE,
        EFFECT_ASSIST_NOT_IN_PERSPECTIVE
    };

    static wxString CurrentDir; //expose current folder name -DJ
    static wxString PlaybackMarker; //keep track of where we are within grid -DJ
    static wxString xlightsFilename; //expose current path name -DJ
    static xLightsXmlFile* CurrentSeqXmlFile; // global object for currently opened XML file
    const wxString &GetShowDirectory() const { return showDirectory; }
    NetInfoClass& GetNetInfo() { return NetInfo; }
    static wxString GetFilename() { return xlightsFilename; }
    void ConversionInit();
    void ConversionError(const wxString& msg);
    void PlayerError(const wxString& msg);
    void SetMediaFilename(const wxString& filename);
    void RenderIseqData(bool bottom_layers, ConvertLogDialog* plog);
    bool IsSequenceDataValid() { return SeqData.IsValidData(); }
    void ClearSequenceData();
    void LoadAudioData(xLightsXmlFile& xml_file);
    void CreateDebugReport(wxDebugReportCompress *report);
    wxString GetThreadStatusReport();
	void SetAudioControls();
    void ImportXLights(const wxFileName &filename);
    void ImportXLights(SequenceElements &se, const std::vector<Element *> &elements, const wxFileName &filename,
        bool allowAllModels = false, bool clearSrc = false);
    void ImportVix(const wxFileName &filename);
    void ImportHLS(const wxFileName &filename);
    void ImportLMS(const wxFileName &filename);
    void ImportLSP(const wxFileName &filename);
    void ImportSuperStar(const wxFileName &filename);
    void SaveWorking();
    void SaveWorkingLayout();

    EffectManager &GetEffectManager() { return effectManager; }

private:
    bool ImportSuperStar(Element *el, wxXmlDocument &doc, int x_size, int y_size,
                         int x_offset, int y_offset, bool average_colors,
                         int imageResizeType, const wxSize &modelSize);
    bool ImportLMS(wxXmlDocument &doc, const wxFileName &filename);

    //(*Handlers(xLightsFrame)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnMenuOpenFolderSelected(wxCommandEvent& event);
    void OnTimer1Trigger(wxTimerEvent& event);
    void OnCheckBoxRunScheduleClick(wxCommandEvent& event);
    void OnButtonAddShowClick(wxCommandEvent& event);
    void OnButtonUpdateShowClick(wxCommandEvent& event);
    void OnButtonDeleteShowClick(wxCommandEvent& event);
    void OnButtonShowDatesChangeClick(wxCommandEvent& event);
    void OnButtonClearLogClick(wxCommandEvent& event);
    void OnButtonSaveLogClick(wxCommandEvent& event);
    void OnMenuItemRenameListSelected(wxCommandEvent& event);
    void OnMenuItemRefreshSelected(wxCommandEvent& event);
    void OnMenuItemCustomScriptSelected(wxCommandEvent& event);
    void OnButtonNetworkChangeClick(wxCommandEvent& event);
    void OnButtonNetworkDeleteClick(wxCommandEvent& event);
    void OnButtonNetworkMoveUpClick(wxCommandEvent& event);
    void OnButtonNetworkMoveDownClick(wxCommandEvent& event);
    void OnGridNetworkBeginDrag(wxListEvent& event);
    void OnButtonAddE131Click(wxCommandEvent& event);
    void OnButtonAddDongleClick(wxCommandEvent& event);
    void OnButtonSaveSetupClick(wxCommandEvent& event);
    void OnBitmapButtonTabInfoClick(wxCommandEvent& event);
    void OnMenuItemDelListSelected(wxCommandEvent& event);
    void OnMenuItemAddListSelected(wxCommandEvent& event);
    void OnButtonLightsOffClick(wxCommandEvent& event);
    void OnCheckBoxLightOutputClick(wxCommandEvent& event);
    void OnButtonStopNowClick(wxCommandEvent& event);
    void OnButtonGracefulStopClick(wxCommandEvent& event);
    void OnButtonSaveScheduleClick(wxCommandEvent& event);
    void OnMenuItemSavePlaylistsSelected(wxCommandEvent& event);
    void OnButtonNetworkDeleteAllClick(wxCommandEvent& event);
    void OnBitmapButtonOpenSeqClick(wxCommandEvent& event);
    void OnBitmapButtonSaveSeqClick(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnMenuItemBackupSelected(wxCommandEvent& event);
    void OnEffectsPanel1Paint(wxPaintEvent& event);
    void OnGrid1SetFocus(wxFocusEvent& event);
    void OnGrid1KillFocus(wxFocusEvent& event);
    void OntxtCtrlSparkleFreqText(wxCommandEvent& event);
    void OnPanelSequencerPaint(wxPaintEvent& event);
    void OnButtonNewSequenceClick(wxCommandEvent& event);
    void OnButtonClickSaveAs(wxCommandEvent& event);
    void OnNotebook1PageChanged1(wxAuiNotebookEvent& event);
    void ChangeMediaDirectory(wxCommandEvent& event);
    void OnAuiToolBarItemPlayButtonClick(wxCommandEvent& event);
    void OnAuiToolBarItemPauseButtonClick(wxCommandEvent& event);
    void OnAuiToolBarItemStopClick(wxCommandEvent& event);
    void OnAuiToolBarItemZoominClick(wxCommandEvent& event);
    void OnAuiToolBarItem_ZoomOutClick(wxCommandEvent& event);
    void OnMenuItemLoadEditPerspectiveSelected(wxCommandEvent& event);
    void OnMenuItemViewSavePerspectiveSelected(wxCommandEvent& event);
    void OnMenu_Settings_SequenceSelected(wxCommandEvent& event);
    void OnMenuItem_File_Open_SequenceSelected(wxCommandEvent& event);
    void OnMenuItem_File_Save_SequenceSelected(wxCommandEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnAuiToolBarItemRenderAllClick(wxCommandEvent& event);
    void OnMenuItem_File_Close_SequenceSelected(wxCommandEvent& event);
    void OnAuiToolBarFirstFrameClick(wxCommandEvent& event);
    void OnAuiToolBarLastFrameClick(wxCommandEvent& event);
    void OnAuiToolBarItemReplaySectionClick(wxCommandEvent& event);
    void ShowHideEffectSettingsWindow(wxCommandEvent& event);
    void ShowHideColorWindow(wxCommandEvent& event);
    void ShowHideLayerTimingWindow(wxCommandEvent& event);
    void ShowHideModelPreview(wxCommandEvent& event);
    void ShowHideEffectDropper(wxCommandEvent& event);
    void ShowHideHousePreview(wxCommandEvent& event);
    void SetIconSize(wxCommandEvent& event);
    void ResetToolbarLocations(wxCommandEvent& event);
    void SetToolIconSize(wxCommandEvent& event);
    void OnMenuItemRenderEraseModeSelected(wxCommandEvent& event);
    void OnMenuItemRenderCanvasModeSelected(wxCommandEvent& event);
    void OnSetGridIconBackground(wxCommandEvent& event);
    void OnSetGridNodeValues(wxCommandEvent& event);
    void OnMenuItemImportEffects(wxCommandEvent& event);
    void SetPlaySpeed(wxCommandEvent& event);
    void OnBitmapButton_Link_DirsClick(wxCommandEvent& event);
    void OnMenuItemRenderOnSave(wxCommandEvent& event);
    void OnNotebook1PageChanging(wxAuiNotebookEvent& event);
    void OnButtonAddNullClick(wxCommandEvent& event);
    void OnMenuItemEffectAssistAlwaysOnSelected(wxCommandEvent& event);
    void OnMenuItemEffectAssistAlwaysOffSelected(wxCommandEvent& event);
    void OnMenuItemEffectAssistToggleModeSelected(wxCommandEvent& event);
    void ShowHidePerspectivesWindow(wxCommandEvent& event);
    void ShowHideDisplayElementsWindow(wxCommandEvent& event);
    void ShowHideEffectAssistWindow(wxCommandEvent& event);
    void OnMenuItem_File_SaveAs_SequenceSelected(wxCommandEvent& event);
    void OnGridNetworkItemActivated(wxListEvent& event);
    void OnMenuDockAllSelected(wxCommandEvent& event);
    void ShowHideBufferSettingsWindow(wxCommandEvent& event);
    void ResetWindowsToDefaultPositions(wxCommandEvent& event);
    void OnActionTestMenuItemSelected(wxCommandEvent& event);
    void OnAuiToolBarItemShowHideEffects(wxCommandEvent& event);
    void OnAuiToolBarItemPasteByTimeClick(wxCommandEvent& event);
    void OnAuiToolBarItemPasteByCellClick(wxCommandEvent& event);
    void OnMenuItemConvertSelected(wxCommandEvent& event);
    void OnMenu_GenerateCustomModelSelected(wxCommandEvent& event);
    void OnPaneClose(wxAuiManagerEvent& event);
    void OnMenuItemPackageDebugFiles(wxCommandEvent& event);
    void OnMenuOpenGLSelected(wxCommandEvent& event);
    void OnTimer_AutoSaveTrigger(wxTimerEvent& event);
    void AutoSaveIntervalSelected(wxCommandEvent& event);
    void OnEffectSettingsTimerTrigger(wxTimerEvent& event);
    void OnMenuItemViewSaveAsPerspectiveSelected(wxCommandEvent& event);
    void OnmBackupOnSaveSelected(wxCommandEvent& event);
    void OnmAltBackupLocationMenuItemSelected(wxCommandEvent& event);
    void OnmAltBackupMenuItemSelected(wxCommandEvent& event);
    void OnmExportModelsMenuItemSelected(wxCommandEvent& event);
    void OnMenuItem_BackupOnLaunchSelected(wxCommandEvent& event);
    void OnMenuItem_ViewLogSelected(wxCommandEvent& event);
    void OnMenuItemCheckSequenceSelected(wxCommandEvent& event);
    void OnSpinCtrl_SyncUniverseChange(wxSpinEvent& event);
    void OnMenuItem_e131syncSelected(wxCommandEvent& event);
    void OnButtonArtNETClick(wxCommandEvent& event);
    void OnMenuItem_Help_ForumSelected(wxCommandEvent& event);
    void OnMenuItem_Help_DownloadSelected(wxCommandEvent& event);
    void OnMenuItem_Help_ReleaseNotesSelected(wxCommandEvent& event);
    void OnMenuItem_Help_Isue_TrackerSelected(wxCommandEvent& event);
    void OnMenuItem_Help_FacebookSelected(wxCommandEvent& event);
    void OnMenuItem_ExportEffectsSelected(wxCommandEvent& event);
    void OnGridNetworkItemRClick(wxListEvent& event);
    void OnGridNetworkItemSelect(wxListEvent& event);
    void OnGridNetworkItemDeselect(wxListEvent& event);
    void OnGridNetworkItemFocused(wxListEvent& event);
    void OnGridNetworkKeyDown(wxListEvent& event);
    void OnMenuItem_FPP_ConnectSelected(wxCommandEvent& event);
    void OnMenuItemShiftEffectsSelected(wxCommandEvent& event);
    //*)

    void DoMenuAction(wxMenuEvent &evt);
	void ShowHideAllSequencerWindows(bool show);
	void ResetAllSequencerWindows();
	void SetEffectAssistWindowState(bool show);
    void UpdateEffectAssistWindow(Effect* effect, RenderableEffect* ren_effect);
    void MaybePackageAndSendDebugFiles();
    void SendReport(const wxString &loc, wxDebugReportCompress &report);
    void AddDebugFilesToReport(wxDebugReport &report);

    //(*Identifiers(xLightsFrame)
    static const long ID_AUITOOLBAR_OPENSHOW;
    static const long ID_AUITOOLBAR_NEWSEQUENCE;
    static const long ID_AUITOOLBAR_OPEN;
    static const long ID_AUITOOLBAR_SAVE;
    static const long ID_AUITOOLBAR_SAVEAS;
    static const long ID_AUITOOLBAR_RENDERALL;
    static const long ID_AUITOOLBAR_MAIN;
    static const long ID_AUITOOLBAR_PLAY_NOW;
    static const long ID_AUITOOLBAR_PAUSE;
    static const long ID_AUITOOLBAR_STOP;
    static const long ID_AUITOOLBAR_FIRST_FRAME;
    static const long ID_AUITOOLBAR_LAST_FRAME;
    static const long ID_AUITOOLBAR_REPLAY_SECTION;
    static const long ID_AUITOOLBAR_PLAY;
    static const long ID_AUITOOLBARITEM2;
    static const long ID_AUITOOLBARITEM5;
    static const long ID_AUITOOLBARITEM7;
    static const long ID_AUITOOLBARITEM3;
    static const long ID_AUITOOLBARITEM1;
    static const long ID_AUITOOLBARITEM4;
    static const long ID_AUITOOLBARITEM6;
    static const long ID_AUITOOLBARITEM8;
    static const long ID_AUIWINDOWTOOLBAR;
    static const long ID_PASTE_BY_TIME;
    static const long ID_PASTE_BY_CELL;
    static const long ID_AUITOOLBAR_EDIT;
    static const long ID_AUITOOLBARITEM14;
    static const long ID_AUITOOLBAR_VIEW;
    static const long ID_BITMAPBUTTON_TAB_INFO;
    static const long ID_BUTTON_STOP_NOW;
    static const long ID_BUTTON_GRACEFUL_STOP;
    static const long ID_BUTTON_LIGHTS_OFF;
    static const long ID_CHECKBOX_LIGHT_OUTPUT;
    static const long ID_AUITOOLBAR_OUTPUT;
    static const long ID_AUIEFFECTSTOOLBAR;
    static const long ID_BUTTON3;
    static const long ID_STATICTEXT4;
    static const long ID_BUTTON_CHANGE_MEDIA_DIR;
    static const long ID_ANY;
    static const long ID_BITMAPBUTTON_Link_Dirs;
    static const long ID_BUTTON_SAVE_SETUP;
    static const long ID_BUTTON_ADD_DONGLE;
    static const long ID_BUTTON_ADD_E131;
    static const long ID_BUTTON1;
    static const long ID_BUTTON2;
    static const long ID_BUTTON_NETWORK_CHANGE;
    static const long ID_BUTTON_NETWORK_DELETE;
    static const long ID_BUTTON_NETWORK_DELETE_ALL;
    static const long ID_STATICTEXT8;
    static const long ID_SPINCTRL1;
    static const long ID_BITMAPBUTTON1;
    static const long ID_BITMAPBUTTON2;
    static const long ID_LISTCTRL_NETWORKS;
    static const long ID_PANEL_SETUP;
    static const long ID_PANEL_PREVIEW;
    static const long ID_TREECTRL1;
    static const long ID_CHECKBOX_RUN_SCHEDULE;
    static const long ID_BUTTON_SAVE_SCHEDULE;
    static const long ID_BUTTON_ADD_SHOW;
    static const long ID_BUTTON_UPDATE_SHOW;
    static const long ID_BUTTON_DELETE_SHOW;
    static const long ID_STATICTEXT2;
    static const long ID_BUTTON_SHOW_DATES_CHANGE;
    static const long ID_STATICTEXT3;
    static const long ID_STATICTEXT_SHOWSTART;
    static const long ID_STATICTEXT5;
    static const long ID_STATICTEXT_SHOWEND;
    static const long ID_PANEL3;
    static const long ID_STATICTEXT1;
    static const long ID_TEXTCTRL_LOG;
    static const long ID_BUTTON_CLEARLOG;
    static const long ID_BUTTON_SAVELOG;
    static const long ID_PANEL2;
    static const long ID_SPLITTERWINDOW1;
    static const long ID_PANEL_CAL;
    static const long ID_PANEL7;
    static const long ID_NOTEBOOK1;
    static const long ID_STATICTEXT6;
    static const long ID_GAUGE1;
    static const long ID_PANEL5;
    static const long ID_STATICTEXT7;
    static const long ID_PANEL1;
    static const long ID_NEW_SEQUENCE;
    static const long ID_OPEN_SEQUENCE;
    static const long IS_SAVE_SEQ;
    static const long ID_SAVE_AS_SEQUENCE;
    static const long ID_CLOSE_SEQ;
    static const long ID_MENUITEM2;
    static const long ID_FILE_BACKUP;
    static const long ID_FILE_ALTBACKUP;
    static const long ID_SHIFT_EFFECTS;
    static const long ID_MENUITEM13;
    static const long ID_MENUITEM_CONVERT;
    static const long ID_MENUITEM_GenerateCustomModel;
    static const long ID_MNU_CHECKSEQ;
    static const long ID_MENU_VIEW_LOG;
    static const long ID_MENUITEM18;
    static const long ID_EXPORT_MODELS;
    static const long ID_MNU_EXPORT_EFFECTS;
    static const long ID_MENU_FPP_CONNECT;
    static const long idMenuSaveSched;
    static const long idMenuAddList;
    static const long idMenuRenameList;
    static const long idMenuDelList;
    static const long ID_MENUITEM1;
    static const long idCustomScript;
    static const long ID_MENUITEM_SAVE_PERSPECTIVE;
    static const long ID_MENUITEM_SAVE_AS_PERSPECTIVE;
    static const long ID_MENUITEM_LOAD_PERSPECTIVE;
    static const long ID_MENUITEM7;
    static const long ID_MENUITEM_DISPLAY_ELEMENTS;
    static const long ID_MENUITEM12;
    static const long ID_MENUITEM3;
    static const long ID_MENUITEM14;
    static const long ID_MENUITEM15;
    static const long ID_MENUITEM16;
    static const long ID_MENUITEM9;
    static const long ID_MENUITEM17;
    static const long ID_MENUITEM_EFFECT_ASSIST_WINDOW;
    static const long ID_MENUITEM_WINDOWS_PERSPECTIVE;
    static const long ID_MENUITEM_WINDOWS_DOCKALL;
    static const long ID_MENUITEM11;
    static const long ID_MENUITEM10;
    static const long ID_PLAY_FULL;
    static const long ID_PLAY_3_4;
    static const long ID_PLAY_1_2;
    static const long ID_PLAY_1_4;
    static const long ID_IMPORT_EFFECTS;
    static const long ID_SEQ_SETTINGS;
    static const long ID_RENDER_ON_SAVE;
    static const long ID_BACKUP_ON_SAVE;
    static const long ID_MENU_BACKUP_ON_LAUNCH;
    static const long ID_ALT_BACKUPLOCATION;
    static const long ID_MENUITEM_ICON_SMALL;
    static const long ID_MENUITEM_ICON_MEDIUM;
    static const long ID_MENUITEM_ICON_LARGE;
    static const long ID_MENUITEM_ICON_XLARGE;
    static const long ID_MENUITEM4;
    static const long ID_MENUITEM_GRID_ICON_XSMALL;
    static const long ID_MENUITEM_GRID_ICON_SMALL;
    static const long ID_MENUITEM_GRID_ICON_MEDIUM;
    static const long ID_MENUITEM_GRID_ICON_LARGE;
    static const long ID_MENUITEM_GRID_ICON_XLARGE;
    static const long ID_MENUITEM6;
    static const long ID_MENUITEM_GRID_ICON_BACKGROUND_ON;
    static const long ID_MENUITEM_GRID_ICON_BACKGROUND_OFF;
    static const long ID_MENUITEM_Grid_Icon_Backgrounds;
    static const long ID_MENUITEM_GRID_NODE_VALUES_ON;
    static const long ID_MENUITEM_GRID_NODE_VALUES_OFF;
    static const long ID_MENUITEM8;
    static const long ID_MENU_CANVAS_ERASE_MODE;
    static const long ID_MENU_CANVAS_CANVAS_MODE;
    static const long ID_MENUITEM_RENDER_MODE;
    static const long ID_MENUITEM_EFFECT_ASSIST_ALWAYS_ON;
    static const long ID_MENUITEM_EFFECT_ASSIST_ALWAYS_OFF;
    static const long ID_MENUITEM_EFFECT_ASSIST_TOGGLE;
    static const long ID_MENUITEM_EFFECT_ASSIST;
    static const long ID_MENU_OPENGL_AUTO;
    static const long ID_MENU_OPENGL_3;
    static const long ID_MENU_OPENGL_2;
    static const long ID_MENU_OPENGL_1;
    static const long ID_MENUITEM19;
    static const long ID_MENUITEM_AUTOSAVE_0;
    static const long ID_MENUITEM_AUTOSAVE_3;
    static const long ID_MENUITEM_AUTOSAVE_10;
    static const long ID_MENUITEM_AUTOSAVE_15;
    static const long ID_MENUITEM_AUTOSAVE_30;
    static const long ID_MENUITEM20;
    static const long ID_E131_Sync;
    static const long ID_MENUITEM5;
    static const long idMenuHelpContent;
    static const long ID_MENU_HELP_FORMUM;
    static const long ID_MENU_HELP_DOWNLOAD;
    static const long ID_MNU_HELP_RELEASE_NOTES;
    static const long ID_MENU_HELP_ISSUE;
    static const long ID_MENU_HELP_FACEBOOK;
    static const long ID_TIMER1;
    static const long ID_TIMER2;
    static const long ID_TIMER_EFFECT_SETTINGS;
    //*)

    static const long ID_PANEL_EFFECTS1;
    static const long ID_PANEL_EFFECTS;
    static const long ID_NOTEBOOK_EFFECTS;

    static const long ID_PLAYER_DIALOG;
    static const long ID_DELETE_EFFECT;
    static const long ID_IGNORE_CLICK;
    static const long ID_PROTECT_EFFECT;
    static const long ID_UNPROTECT_EFFECT;
    static const long ID_RANDOM_EFFECT;
    static const long ID_COPYROW_EFFECT; //copy random effect across row -DJ
    static const long ID_CLEARROW_EFFECT; //clear all effects on this row -DJ

    //(*Declarations(xLightsFrame)
    xlAuiToolBar* OutputToolBar;
    wxButton* ButtonAddE131;
    wxMenuItem* MenuItemViewSavePerspective;
    wxButton* ButtonArtNET;
    wxMenuItem* QuitMenuItem;
    wxMenuItem* MenuItem33;
    wxMenuItem* MenuItemLoadEditPerspective;
    wxMenuItem* MenuItemGridNodeValuesOff;
    wxCheckBox* CheckBoxRunSchedule;
    wxMenuItem* MenuItem40;
    wxButton* ButtonClearLog;
    wxBitmapButton* BitmapButtonMoveNetworkDown;
    wxMenu* ToolIconSizeMenu;
    wxMenuItem* MenuItem_File_Open_Sequence;
    wxAuiManager* m_mgr;
    wxStaticText* StaticText2;
    wxMenuItem* MenuItem_ExportEffects;
    wxAuiManager* MainAuiManager;
    wxMenuItem* MenuItemRenderCanvasMode;
    wxStaticText* StaticTextShowEnd;
    wxMenuItem* MenuItemGridNodeValuesOn;
    wxMenu* Menu3;
    wxTextCtrl* TextCtrlLog;
    wxMenu* MenuItemRenderMode;
    wxButton* Button_Change_Media_Dir;
    wxMenuItem* MenuItemRefresh;
    wxMenuItem* mAltBackupLocationMenuItem;
    wxMenuItem* MenuItemShiftEffects;
    wxMenuItem* MenuItem_File_Save_Sequence;
    wxMenuItem* MenuItem36;
    wxMenuItem* MenuItemCheckSequence;
    wxButton* ButtonNetworkDeleteAll;
    wxTimer EffectSettingsTimer;
    wxMenuItem* MenuItemGridIconBackgroundOn;
    wxMenuItem* MenuItem_File_Close_Sequence;
    wxStaticText* StaticTextShowStart;
    wxPanel* AUIStatusBar;
    xlAuiToolBar* ViewToolBar;
    wxMenuItem* MenuItem37;
    wxStaticText* StaticText1;
    wxMenuItem* MenuItem_BackupOnLaunch;
    wxMenuItem* MenuItem32;
    wxMenuItem* MenuItem_ViewZoomIn;
    wxPanel* PanelPreview;
    xLightsTimer Timer1;
    wxMenuItem* MenuItem_Help_Isue_Tracker;
    wxStaticText* StaticText3;
    xlAuiToolBar* PlayToolBar;
    wxMenu* Menu1;
    wxFlexGridSizer* GaugeSizer;
    wxPanel* PanelSequencer;
    wxButton* ButtonSaveLog;
    wxMenuItem* MenuItem42;
    wxPanel* Panel3;
    wxMenuItem* MenuItemEffectAssistAlwaysOn;
    wxMenu* MenuItem7;
    wxButton* ButtonAddDongle;
    wxMenuItem* MenuItem_ViewZoomOut;
    wxBitmapButton* BitmapButtonMoveNetworkUp;
    wxMenuItem* MenuItemBackup;
    xlAuiToolBar* WindowMgmtToolbar;
    wxMenuItem* Menu_GenerateCustomModel;
    wxMenuItem* MenuItem38;
    wxMenuItem* MenuItemViewSaveAsPerspective;
    wxMenuItem* MenuItemRenderEraseMode;
    wxMenuItem* MenuItem3;
    wxMenuItem* mExportModelsMenuItem;
    wxMenuItem* MenuItemConvert;
    wxButton* ButtonNetworkChange;
    wxMenuItem* MenuItem_Help_Facebook;
    wxButton* ButtonAddNull;
    wxMenu* AutoSaveMenu;
    wxMenuItem* MenuItem_Help_ReleaseNotes;
    wxMenuItem* Menu_Settings_Sequence;
    wxMenu* MenuSettings;
    wxButton* ButtonNetworkDelete;
    wxBitmapButton* BitmapButton_Link_Dirs;
    wxMenuItem* MenuItemEffectAssistAlwaysOff;
    wxMenuItem* MenuItem_FPP_Connect;
    wxStaticText* StaticText5;
    wxTreeCtrl* ListBoxSched;
    wxListCtrl* GridNetwork;
    wxStaticText* ShowDirectoryLabel;
    wxPanel* PanelCal;
    wxMenu* MenuFile;
    wxDirDialog* DirDialog1;
    wxAuiNotebook* Notebook1;
    wxMenuItem* MenuItem35;
    wxButton* ButtonDeleteShow;
    wxButton* ButtonShowDatesChange;
    wxMenu* OpenGLMenu;
    wxStaticText* StatusText;
    wxMenuItem* MenuItem_File_SaveAs_Sequence;
    xlAuiToolBar* MainToolBar;
    wxMenuItem* MenuItemEffectAssistToggleMode;
    wxTimer Timer_AutoSave;
    wxStaticText* MediaDirectoryLabel;
    wxMenuItem* mRenderOnSaveMenuItem;
    wxMenuItem* MenuItem41;
    wxPanel* PanelSetup;
    wxMenuItem* MenuItem_e131sync;
    wxButton* ButtonSaveSetup;
    wxPanel* Panel2;
    wxMenuItem* mBackupOnSaveMenuItem;
    wxMenuItem* MenuItemSavePlaylists;
    wxMenu* MenuItemPerspectives;
    wxButton* ButtonUpdateShow;
    wxMenu* GridSpacingMenu;
    wxStaticText* FileNameText;
    wxMenuItem* mAltBackupMenuItem;
    wxMenuItem* MenuItem_Help_Download;
    wxMenuItem* MenuItem16;
    wxMenuItem* MenuItem34;
    wxSplitterWindow* SplitterWindow1;
    wxMenuItem* MenuItem43;
    wxMenuItem* MenuItemEffectAssistWindow;
    wxMenuItem* ActionTestMenuItem;
    wxStaticText* StaticText4;
    wxMenu* MenuItem18;
    wxMenu* MenuItem1;
    wxGauge* ProgressBar;
    xlAuiToolBar* EffectsToolBar;
    wxButton* ButtonSaveSchedule;
    wxMenuItem* MenuItem_Help_Forum;
    wxButton* ButtonAddShow;
    wxGridBagSizer* StatusBarSizer;
    wxMenu* MenuView;
    wxMenuItem* MenuItem_ViewLog;
    wxMenu* AudioMenu;
    xlAuiToolBar* EditToolBar;
    wxMenuItem* MenuItemGridIconBackgroundOff;
    wxMenuBar* MenuBar;
    wxSpinCtrl* SpinCtrl_SyncUniverse;
    //*)

    AUIToolbarButtonWrapper *CheckBoxLightOutput;
    AUIToolbarButtonWrapper *ButtonGracefulStop;
    AUIToolbarButtonWrapper *ButtonPasteByTime;
    AUIToolbarButtonWrapper *ButtonPasteByCell;

    wxBitmap pauseIcon;
    wxBitmap playIcon;
    bool previewLoaded;
    bool previewPlaying;
    wxFileName networkFile;
    wxFileName scheduleFile;
    PlayerFrame* PlayerDlg;
    wxArrayString mru;  // most recently used directories
    wxMenuItem* mru_MenuItem[MRU_LENGTH];
    wxXmlDocument NetworkXML;
    long DragRowIdx;
    wxListCtrl* DragListBox;
    bool UnsavedNetworkChanges;
    bool UnsavedPlaylistChanges;
    wxColor mDefaultNetworkSaveBtnColor;
    int mSavedChangeCount;
    int mLastAutosaveCount;
    wxDateTime starttime;
    play_modes play_mode;
    NetInfoClass NetInfo;
    ModelPreview* modelPreview;
    EffectManager effectManager;
    int effGridPrevX;
    int effGridPrevY;


    void DoBackup(bool prompt = true, bool startup = false, bool forceallfiles = false);
    void DoAltBackup(bool prompt = true);
    void SetPlayMode(play_modes newmode);
    static double rand01();
    bool EnableOutputs();
    void EnableNetworkChanges();
    void AllLightsOff();
    void InitEffectsPanel(EffectsPanel* panel);

    // setup
    void OnMenuMRU(wxCommandEvent& event);
    bool SetDir(const wxString& dirname);
    bool PromptForShowDirectory();
    void UpdateNetworkList(bool updateModels);
    long GetNetworkSelection();
    void MoveNetworkRow(int fromRow, int toRow);
    void OnGridNetworkDragQuit(wxMouseEvent& event);
    void OnGridNetworkDragEnd(wxMouseEvent& event);
    void SetupDongle(wxXmlNode* e, int after = -1);
    void SetupE131(wxXmlNode* e, int after = -1);
    void SetupArtNet(wxXmlNode* e, int after = -1);
    void SetupNullOutput(wxXmlNode* e, int after = -1);
    bool SaveNetworksFile();
    void NetworkChange();
    std::list<wxXmlNode> GetOutputsForController(const std::string ip);
    std::list<int> GetSelectedOutputs(wxString& ip);
    void UploadFPPBridgeInput();
    void UploadFPPBridgeOutput();
    void UploadFalconInput();
    void UploadFalconOutput();

    void DeleteSelectedNetworks();
    void ActivateSelectedNetworks(bool active);
    void ChangeSelectedNetwork();
    wxXmlNode* GetOutput(int num);
    bool AllSelectedSupportIP();
    bool AllSelectedSupportChannels();
    void UpdateSelectedIPAddresses();
    void UpdateSelectedChannels();
    void UpdateSelectedDescriptions();

    void OnProgressBarDoubleClick(wxMouseEvent& event);
    RenderProgressInfo *renderProgressInfo;
    RenderProgressDialog *renderProgressDialog;

    wxString mediaFilename;
    wxString showDirectory;
    wxString mediaDirectory;
    SeqDataType SeqData;

    wxArrayString ChannelNames;
    wxArrayInt ChannelColors;
    long seekPoint;

    int mHitTestNextSelectModelIndex;
    wxString mBackgroundImage;
    int mBackgroundBrightness;
    bool mScaleBackgroundImage = false;
    std::string mStoredLayoutGroup;

    // convert
public:
    bool UnsavedRgbEffectsChanges;

    void ClearLastPeriod();
    void WriteVirFile(const wxString& filename, long numChans, long numPeriods, SeqDataType *dataBuf); //       Vixen *.vir
    void WriteHLSFile(const wxString& filename, long numChans, long numPeriods, SeqDataType *dataBuf);  //      HLS *.hlsnc
    void WriteLcbFile(const wxString& filename, long numChans, long numPeriods, SeqDataType *dataBuf);  //      LOR *.lcb
    void WriteLSPFile(const wxString& filename, long numChans, long numPeriods, SeqDataType *dataBuf, int cpn);  //      LSP UserPatterns.xml
    wxString base64_encode();
    int base64_decode(const wxString& encoded_string, std::vector<unsigned char> &data);
    void ReadXlightsFile(const wxString& FileName, wxString *mediaFilename = NULL);
    void ReadFalconFile(const wxString& FileName, ConvertDialog* convertdlg); // = NULL);
    void WriteFalconPiFile(const wxString& filename); //  Falcon Pi Player *.pseq
    wxXmlNode* GetNetworksXMLRoot() { return NetworkXML.GetRoot(); };

private:

    void WriteFalconPiModelFile(const wxString& filename, long numChans, long numPeriods,
                                SeqDataType *dataBuf, int startAddr, int modelSize); //Falcon Pi sub sequence .eseq
    void WriteVideoModelFile(const wxString& filename, long numChans, long numPeriods,
        SeqDataType *dataBuf, int startAddr, int modelSize, Model* model, bool compressed); //.avi file

    JobPool jobPool;

    // schedule
    wxDateTime ShowStartDate,ShowEndDate;
    wxArrayString ShowEvents;
    SeqPlayerStates SeqPlayerState;
    wxString LastMoDay;
    long DelayAfterPlayMSEC;

    void AddShow(const wxDateTime& d, const wxString& StartStop, const wxString& Playlist);
    void DisplaySchedule();
    int DisplayScheduleOneDay(const wxDateTime& d, const wxTreeItemId& root);
    void PopulateShowDialog(AddShowDialog& dialog, wxSortedArrayString& SelectedDates);
    void UnpackSchedCode(const wxString& SchedCode, wxString& StartTime, wxString& EndTime, wxString& RepeatOptions, wxString& Playlist);
    void ForceScheduleCheck();
    void CheckRunSchedule();
    long GetSelectedItem(wxListCtrl* ListBoxPlay);
    void UpdateShowDates(const wxDateTime& NewStart, const wxDateTime NewEnd);
    bool DisplayAddShowDialog(AddShowDialog& dialog);
    int DeleteSelectedShows();
    void CheckSchedule();
    void LoadScheduleFile();
    void LoadSchedule(wxXmlNode* n);
    void LoadPlaylists(wxXmlNode* n);
    void LoadPlaylist(wxXmlNode* n);
    int AddPlaylist(const wxString& name);
    wxString OnOffString(bool b);
    wxString CreateScript(wxString ListName, bool Repeat, bool FirstItemOnce, bool LastItemOnce, bool LightsOff, bool Random);
    bool PlayCurrentXlightsFile();
    void RunPlaylist(int nbidx, wxString& script);
    void SendToLogAndStatusBar(const wxString& msg);
    void ScanForFiles();
    long DiffSeconds(wxString& strTime, wxTimeSpan& tsCurrent);
    int Time2Seconds(const wxString& hhmm);
    void ResetTimer(SeqPlayerStates newstate, long OffsetMsec=0);
    void SaveScheduleFile();
    void OnButtonPlaylistAddClick(wxCommandEvent& event);
    void OnButtonPlaylistAddAllClick(wxCommandEvent& event);
    void OnButtonPlaylistDeleteClick(wxCommandEvent& event);
    void OnButtonPlaylistDeleteAllClick(wxCommandEvent& event);
    void OnButtonRemoveScriptClick(wxCommandEvent& event);
    void OnButtonRunPlaylistClick(wxCommandEvent& event);
    void OnButtonPlayItemClick(wxCommandEvent& event);
    void OnButtonUpClick(wxCommandEvent& event);
    void OnButtonDownClick(wxCommandEvent& event);
    void OnButtonSetDelayClick(wxCommandEvent& event);
    void OnFileTypeButtonClicked(wxCommandEvent& event);
    void OnPlayListBeginDrag(wxListEvent& event);
    void OnPlayListDragEnd(wxMouseEvent& event);
    void OnPlayListDragQuit(wxMouseEvent& event);
    void OnTimerPlaylist(long msec);
    void OnNetworkPopup(wxCommandEvent &event);

    // sequence
    void LoadEffectsFile();
    wxString LoadEffectsFileNoCheck();
    void CreateDefaultEffectsXml();
    void TimerRgbSeq(long msec);
    void SetChoicebook(wxChoicebook* cb, const wxString& PageName);
    wxString GetXmlSetting(const wxString& settingName,const wxString& defaultValue);

    void DisplayXlightsFilename(const wxString& filename);
    int ChooseRandomEffect();

public:
    bool InitPixelBuffer(const std::string &modelName, PixelBufferClass &buffer, int layerCount, bool zeroBased = false);
    Model *GetModel(const std::string& name);
    void RenderGridToSeqData(std::function<void()>&& callback);
    void UpdateRenderStatus();
    bool RenderEffectFromMap(Effect *effect, int layer, int period, const SettingsMap& SettingsMap,
                             PixelBufferClass &buffer, bool &ResetEffectState,
                             bool bgThread = false, RenderEvent *event = NULL);
    void RenderEffectOnMainThread(RenderEvent *evt);
    void RenderEffectForModel(const std::string &model, int startms, int endms, bool clear = false);

    void RenderRange(RenderCommandEvent &cmd);
    void RenderDone();

    void EnableSequenceControls(bool enable);
    SequenceElements& GetSequenceElements() { return mSequenceElements; }
    TimingElement* AddTimingElement(const std::string& name);
    void DeleteTimingElement(const std::string& name);
    void RenameTimingElement(const std::string& old_name, const std::string& new_name);
    void ImportTimingElement();
    void ExecuteImportTimingElement(wxCommandEvent &command);
    void ExecuteImportNotes(wxCommandEvent &command);
    void ConvertDataRowToEffects(wxCommandEvent &command);
    void ConvertDataRowToEffects(EffectLayer *layer, xlColorVector &colors, int frameTime);
    void PromoteEffects(wxCommandEvent &command);
    void PromoteEffects(ModelElement *element);
    wxXmlNode* CreateEffectNode(wxString& name);
    void UpdateEffectNode(wxXmlNode* node);
    void ApplyEffectsPreset(wxString& data, const wxString &pasteDataVersion);
    void RenameModelInViews(const std::string old_name, const std::string& new_name);
    bool RenameModel(const std::string old_name, const std::string& new_name);
    void UpdateSequenceLength();

    void SetSequenceEnd(int ms);
    void UpdateRenderMode();
    void SetFrequency(int frequency);
    void RenderAll();

    void SetXmlSetting(const wxString& settingName,const wxString& value);
    unsigned int GetMaxNumChannels();

protected:
    bool SeqLoadXlightsFile(const wxString& filename, bool ChooseModels);
    bool SeqLoadXlightsFile(xLightsXmlFile& xml_file, bool ChooseModels);
    void ResetEffectsXml();
    void SeqLoadXlightsXSEQ(const wxString& filename);
    std::string CreateEffectStringRandom(std::string &settings, std::string &palette);
    void CopyFiles(const wxString& wildcard, wxDir& srcDir, wxString& targetDirName, bool forceallfiles);
    void BackupDirectory(wxString targetDirName, bool forceallfiles);
    void NewSequence();
    void OpenSequence(wxString passed_filename, ConvertLogDialog* plog);
    void SaveSequence();
    void SaveAsSequence();
    bool CloseSequence();
    void OpenRenderAndSaveSequences(const wxArrayString &filenames);
    void AddAllModelsToSequence();
    void ShowPreviewTime(long ElapsedMSec);
    void PreviewOutput(int period);
    void TimerOutput(int period);
    void UpdateChannelNames();
    void StopNow(void);

    bool Grid1HasFocus; //cut/copy/paste handled differently with grid vs. other text controls -DJ
    wxXmlDocument EffectsXml;
    wxXmlNode* EffectsNode;
    wxXmlNode* PalettesNode;
    wxXmlNode* ViewsNode;
    wxXmlNode* PerspectivesNode;
public:
    wxXmlNode* ModelsNode;
    wxXmlNode* ModelGroupsNode;
    wxXmlNode* LayoutGroupsNode;
    wxXmlNode* GetViewsNode() const {return ViewsNode;}

private:
    wxXmlNode* SettingsNode;

    bool MixTypeChanged;
    bool FadesChanged;
    long SeqBaseChannel;
    bool SeqChanCtrlBasic;
    bool SeqChanCtrlColor;
	bool mLoopAudio;

    bool mResetToolbars;
    bool mRenderOnSave;
    bool mBackupOnSave;
    bool mBackupOnLaunch;
    bool me131Sync;
    wxString mAltBackupDir;
    int mIconSize;
    int mGridSpacing;
    bool mGridIconBackgrounds;
    bool mGridNodeValues;
    int mEffectAssistMode;
	bool mRendering;
    int AutoSaveInterval;

    Model *playModel;
    int playType;
    int playStartMS;
    std::list<FPSEvent> fpsEvents;
    float _fps;
    int playStartTime;
    int playOffsetTime;
    int playEndTime;
    bool replaySection;
    double playSpeed;
    bool playAnimation;

    std::string selectedEffectName;
    std::string selectedEffectString;
    std::string selectedEffectPalette;
    Effect *selectedEffect;

    std::string lastPlayEffect;
    double mPointSize = 2.0;

    int NextGridRowToPlay;
    int SeqPlayColumn;

	wxArrayString ChNames;
	int _totalChannels;

    int PlaybackPeriod; //used to be able to record the frame being played in an animation preview

public:
    //stuf used by layoutPanel for now
    void SetPreviewBackgroundScaled(bool scaled);
    void SetPreviewSize(int width, int height);
    void SetPreviewBackgroundImage(const wxString &filename);
    const wxString & GetDefaultPreviewBackgroundImage();
    bool GetDefaultPreviewBackgroundScaled();
    int GetDefaultPreviewBackgroundBrightness();
    void SetPreviewBackgroundBrightness(int i);
    void UpdatePreview();
    void UpdateModelsList();
    void RowHeadingsChanged( wxCommandEvent& event);
    int GetTotalChannels() const { return _totalChannels; };
    void ForceSequencerRefresh();
    void RefreshLayout();
    void AddPreviewOption(LayoutGroup* grp);
    void RemovePreviewOption(LayoutGroup* grp);
    ModelPreview* GetLayoutPreview() const {return modelPreview;}
    void SetStoredLayoutGroup(const std::string &group);
    const std::string& GetStoredLayoutGroup() const {return mStoredLayoutGroup;}

private:

    wxXmlNode* mCurrentPerpective;
    std::vector<bool> savedPaneShown;

    // New Sequencer variables and methods
    SequenceElements mSequenceElements;

    MainSequencer* mainSequencer;
    ModelPreview * sPreview1;
    ModelPreview * sPreview2;
    LayoutPanel *layoutPanel;
    EffectAssist* sEffectAssist;
    ColorPanel* colorPanel;
    TimingPanel* timingPanel;
    PerspectivesPanel* perspectivePanel;
    EffectIconPanel *effectPalettePanel;
    BufferPanel *bufferPanel;
    DisplayElementsPanel *displayElementsPanel;


    int mMediaLengthMS;
    bool mSequencerInitialize = false;
    // Methods
    void InitSequencer();
    void CreateSequencer();
    void StopSequence();
    // Events
    void Zoom( wxCommandEvent& event);
    void Scroll( wxCommandEvent& event);
    void WindowResized( wxCommandEvent& event);
    void TimeSelected( wxCommandEvent& event);
    void MousePositionUpdated( wxCommandEvent& event);
    void SelectedEffectChanged( SelectedEffectChangedEvent& event);
    void SelectedRowChanged( wxCommandEvent& event);
    void EffectChanged( wxCommandEvent& event);
    void UnselectedEffect( wxCommandEvent& event);
    void EffectDroppedOnGrid(wxCommandEvent& event);
    void PlayModelEffect(wxCommandEvent& event);
    void UpdateEffect(wxCommandEvent& event);
    void UpdateEffectPalette(wxCommandEvent& event);
    void ForceSequencerRefresh(wxCommandEvent& event);
    void LoadPerspective(wxCommandEvent& event);
    void PerspectivesChanged(wxCommandEvent& event);
    void PlayModel(wxCommandEvent& event);
    void CopyModelEffects(wxCommandEvent& event);
    void PasteModelEffects(wxCommandEvent& event);
    void ModelSelected(wxCommandEvent& event);
    void PlaySequence(wxCommandEvent& event);
    void PauseSequence(wxCommandEvent& event);
    void StopSequence(wxCommandEvent& event);
    void SequenceFirstFrame(wxCommandEvent& event);
    void SequenceLastFrame(wxCommandEvent& event);
    void SequenceReplaySection(wxCommandEvent& event);
    void TogglePlay(wxCommandEvent& event);
    void ExportModel(wxCommandEvent& event);
    void ShowDisplayElements(wxCommandEvent& event);
    void ShowHidePreviewWindow(wxCommandEvent& event);
    void ShowHideAllPreviewWindows(wxCommandEvent& event);
    void ShowHideSync();

    std::map<int, std::list<float>> LoadPolyphonicTranscription(AudioManager* audio, int intervalMS);
    std::map<int, std::list<float>> LoadAudacityFile(std::string file, int intervalMS);
    std::map<int, std::list<float>> LoadMIDIFile(std::string file, int intervalMS, int speedAdjust, int startAdjustMS, std::string track);
    std::map<int, std::list<float>> LoadMusicXMLFile(std::string file, int intervalMS, int speedAdjust, int startAdjustMS, std::string track);
    void CreateNotes(EffectLayer* el, std::map<int, std::list<float>>& notes, int interval, int frames);
    std::string CreateNotesLabel(const std::list<float>& notes) const;
    void CheckSequence(bool display);
    bool CheckStart(wxFile& f, const std::string& startmodel, std::list<std::string>& seen, std::string& nextmodel);

    void CheckForValidModels();
    void ExportModels(wxString filename);
    void ExportEffects(wxString filename);
    int ExportElement(wxFile& f, Element* e, std::map<std::string, int>& effectfrequency, std::map<std::string, int>& effectTotalTime);
    int ExportNodes(wxFile& f, StrandElement* e, NodeLayer* nl, int n, std::map<std::string, int>& effectfrequency, std::map<std::string, int>& effectTotalTime);

    void LoadSequencer(xLightsXmlFile& xml_file);
    void LoadPerspective(wxXmlNode *p);

    void CheckForAndCreateDefaultPerpective();
    void ResizeAndMakeEffectsScroll();
    void ResizeMainSequencer();
    std::string GetEffectTextFromWindows(std::string &palette);
    void SetSyncUniverse(int syncUniverse);

    void EnableToolbarButton(wxAuiToolBar* toolbar,int id, bool enable);
    // Panels
    TopEffectsPanel* effectsPnl;
    EffectsPanel* EffectsPanel1;
    wxFlexGridSizer* FlexGridEffects;
    std::set<int> LorTimingList; // contains a list of period numbers, set by ReadLorFile()

//add lock/unlock/random state flags -DJ
//these could be used to make fields read-only, but initially they are just used for partially random effects
//#include <unordered_map>
    typedef enum { Normal, Locked, Random } EditState;
    std::unordered_map<std::string, EditState> buttonState;
    bool isRandom_(wxControl* ctl, const char*debug);

    wxMenu* MenuItemPreviews;
    wxMenuItem* MenuItemPreviewSeparator;
    static const long ID_MENU_ITEM_PREVIEWS;
    static const long ID_MENU_ITEM_PREVIEWS_SHOW_ALL;

    static const long ID_NETWORK_ADDUSB;
    static const long ID_NETWORK_ADDNULL;
    static const long ID_NETWORK_ADDE131;
    static const long ID_NETWORK_ADDARTNET;
    static const long ID_NETWORK_BEIPADDR;
    static const long ID_NETWORK_BECHANNELS;
    static const long ID_NETWORK_BEDESCRIPTION;
    static const long ID_NETWORK_ADD;
    static const long ID_NETWORK_BULKEDIT;
    static const long ID_NETWORK_DELETE;
    static const long ID_NETWORK_ACTIVATE;
    static const long ID_NETWORK_DEACTIVATE;
    static const long ID_NETWORK_OPENCONTROLLER;
    static const long ID_NETWORK_UPLOADCONTROLLER;
    static const long ID_NETWORK_UCOUTPUT;
    static const long ID_NETWORK_UCINPUT;
    static const long ID_NETWORK_UCIFPPB;
    static const long ID_NETWORK_UCOFPPB;
    static const long ID_NETWORK_UCIFALCON;
    static const long ID_NETWORK_UCOFALCON;

#define isRandom(ctl)  isRandom_(ctl, #ctl) //(buttonState[std::string(ctl->GetName())] == Random)

    DECLARE_EVENT_TABLE()
    friend class xLightsApp; //kludge: allow xLightsApp to call OnPaneNutcrackerChar -DJ
public:
    std::vector<Model *> PreviewModels;
    std::vector<LayoutGroup *> LayoutGroups;
    std::vector<ModelPreview *> PreviewWindows;
    ModelManager AllModels;
    static wxXmlNode* FindNode(wxXmlNode* parent, const wxString& tag, const wxString& attr, const wxString& value, bool create = false);

    wxString GetSeqXmlFileName();
	void PlaySequence();
    static std::string DecodeMidi(int midi);

    EffectTreeDialog *EffectTreeDlg;

};

#endif // XLIGHTSMAIN_H
