#include "xLightsMain.h"
#include "SeqSettingsDialog.h"
#include "FileConverter.h"
#include "DataLayer.h"

#include "LMSImportChannelMapDialog.h"
#include "xLightsImportChannelMapDialog.h"
#include "SuperStarImportDialog.h"
#include "SaveChangesDialog.h"
#include "ConvertLogDialog.h"

#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/tokenzr.h>
#include <wx/config.h>

#include "osxMacUtils.h"

void xLightsFrame::AddAllModelsToSequence()
{
    std::string models_to_add = "";
    bool first_model = true;
    for(wxXmlNode* e=ModelGroupsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "modelGroup")
        {
            wxString name=e->GetAttribute("name");
            if (!mSequenceElements.ElementExists(name.ToStdString(), 0))
            {
                if( !first_model ) {
                    models_to_add += ",";
                }
                models_to_add += name;
                first_model = false;
            }
        }
    }
    for(wxXmlNode* e=ModelsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "model")
        {
            wxString name=e->GetAttribute("name");
            if (!mSequenceElements.ElementExists(name.ToStdString(), 0))
            {
                if( !first_model ) {
                    models_to_add += ",";
                }
                models_to_add += name;
                first_model = false;
            }
        }
    }
    mSequenceElements.AddMissingModelsToSequence(models_to_add);
}

void xLightsFrame::NewSequence()
{
    // close any open sequences
    if (!CloseSequence()) {
        return;
    }

    // assign global xml file object
    wxFileName xml_file;
    xml_file.SetPath(CurrentDir);
    CurrentSeqXmlFile = new xLightsXmlFile(xml_file);

    SeqSettingsDialog setting_dlg(this, CurrentSeqXmlFile, mediaDirectory, wxT(""), true);
    setting_dlg.Fit();
    int ret_code = setting_dlg.ShowModal();
    if( ret_code == wxID_CANCEL )
    {
        delete CurrentSeqXmlFile;
        CurrentSeqXmlFile = NULL;
        return;
    }
	else
	{
        if (CurrentSeqXmlFile->GetMedia() != NULL)
		{
			if (CurrentSeqXmlFile->GetMedia()->GetFrameInterval() < 0)
			{
				CurrentSeqXmlFile->GetMedia()->SetFrameInterval(CurrentSeqXmlFile->GetSequenceTimingAsInt());
			}
		}
	}

    // load media if available
    if( CurrentSeqXmlFile->GetSequenceType() == "Media" && CurrentSeqXmlFile->HasAudioMedia() )
    {
        SetMediaFilename(CurrentSeqXmlFile->GetMedia()->FileName());
    }

    wxString mss = CurrentSeqXmlFile->GetSequenceTiming();
    int ms = atoi(mss.c_str());

    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    logger_base.info("New sequence created Type %s Timing %dms.", (const char *)(CurrentSeqXmlFile->GetSequenceType().c_str()), ms);

    LoadSequencer(*CurrentSeqXmlFile);
    CurrentSeqXmlFile->SetSequenceLoaded(true);
    std::string new_timing = "New Timing";
    CurrentSeqXmlFile->AddNewTimingSection(new_timing, this);
    mSequenceElements.AddTimingToAllViews(new_timing);
    MenuItem_File_Save_Sequence->Enable(true);
    MenuItem_File_Close_Sequence->Enable(true);

    unsigned int max = GetMaxNumChannels();
    if( (max > SeqData.NumChannels()) ||
        (CurrentSeqXmlFile->GetSequenceDurationMS() / ms) > SeqData.NumFrames() )
    {
        SeqData.init(max, mMediaLengthMS / ms, ms);
    }
    else
    {
        SeqData.init(max, CurrentSeqXmlFile->GetSequenceDurationMS() / ms, ms);
    }

    // we can render now the sequence data buffers are initialised
    if (ret_code == NEEDS_RENDER)
    {
        RenderAll();
    }

    Timer1.Start(SeqData.FrameTime(), wxTIMER_CONTINUOUS);
    displayElementsPanel->Initialize();
    const std::string view = setting_dlg.GetView();
    if( view == "All Models" )
    {
        AddAllModelsToSequence();
    }
    else if( view != "Empty" )
    {
        displayElementsPanel->SelectView(view);
    }
	SetAudioControls();
    Notebook1->SetSelection(Notebook1->GetPageIndex(PanelSequencer));
}

static wxFileName mapFileName(const wxFileName &orig) {
    if (orig.GetDirCount() == 0) {
        //likely a filename from windows on Mac/Linux or vice versa
        int idx = orig.GetFullName().Last('\\');
        if (idx == -1) {
            idx = orig.GetFullName().Last('/');
        }
        if (idx != -1) {
            return wxFileName(orig.GetFullName().Left(idx),
                              orig.GetFullName().Right(orig.GetFullName().Length() - idx - 1));
        }
    }
    return orig;
}

void xLightsFrame::OpenSequence(const wxString passed_filename, ConvertLogDialog* plog)
{
    bool loaded_xml = false;
    bool loaded_fseq = false;
    wxString filename;
    wxString wildcards = "XML files (*.xml)|*.xml|FSEQ files (*.fseq)|*.fseq";
    if (passed_filename.IsEmpty()) {
        filename = wxFileSelector("Choose sequence file to open", CurrentDir, wxEmptyString, "*.xml", wildcards, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    } else {
        filename = passed_filename;
    }
    if ( !filename.empty() )
    {
        // close any open sequences
        if (!CloseSequence()) {
            return;
        }

        wxStopWatch sw; // start a stopwatch timer

        wxFileName selected_file(filename);
        wxFileName fseq_file = selected_file;
        fseq_file.SetExt("fseq");
        wxFileName xml_file = selected_file;
        xml_file.SetExt("xml");
        wxFileName media_file;

        // load the fseq data file if it exists
        xlightsFilename = fseq_file.GetFullPath();
        if( fseq_file.FileExists() )
        {
            if (plog != NULL)
            {
                plog->Show(true);
            }
            wxString mf;
            ConvertParameters read_params(xlightsFilename,                              // input filename
                                          SeqData,                                      // sequence data object
                                          GetNetInfo(),                                 // global network info
                                          ConvertParameters::READ_MODE_LOAD_MAIN,       // file read mode
                                          this,                                         // xLights main frame
                                          NULL,
                                          plog,
                                          &mf );                                        // media filename

            FileConverter::ReadFalconFile(read_params);
            if( mf != "" )
            {
                media_file = mapFileName(wxFileName::FileName(mf));
            }
            DisplayXlightsFilename(xlightsFilename);
            SeqBaseChannel=1;
            SeqChanCtrlBasic=false;
            SeqChanCtrlColor=false;
            loaded_fseq = true;
        }

        // assign global xml file object
        CurrentSeqXmlFile = new xLightsXmlFile(xml_file);

        // open the xml file so we can see if it has media
        CurrentSeqXmlFile->Open(GetShowDirectory());

        // if fseq didn't have media check xml
        if (CurrentSeqXmlFile->GetMediaFile() != "")
        {
            media_file = mapFileName(CurrentSeqXmlFile->GetMediaFile());
            ObtainAccessToURL(media_file.GetFullPath().ToStdString());
        }

        // still no media file?  look for an XSEQ file and load if found
        if( !wxFileName(media_file).Exists() || !wxFileName(media_file).IsFileReadable() )
        {
            wxFileName xseq_file = selected_file;
            xseq_file.SetExt("xseq");
            if( xseq_file.FileExists() )
            {
                wxString mf;
                ReadXlightsFile(xseq_file.GetFullPath(), &mf);
                if( mf != "" )
                {
                    media_file = mapFileName(wxFileName::FileName(mf));
                    ObtainAccessToURL(media_file.GetFullPath().ToStdString());
                }
                DisplayXlightsFilename(xlightsFilename);
                SeqBaseChannel=1;
                SeqChanCtrlBasic=false;
                SeqChanCtrlColor=false;
            }
        }

        // double-check file existence
        if( !wxFileName(media_file).Exists() || !wxFileName(media_file).IsFileReadable())
        {
            wxFileName detect_media(media_file);

            // search media directory
            detect_media.SetPath(mediaDirectory);
            if( detect_media.FileExists() )
            {
                media_file = detect_media;
            }
            else
            {
                // search selected file directory
                detect_media.SetPath(selected_file.GetPath());
                if( detect_media.FileExists() )
                {
                    media_file = detect_media;
                }
            }
        }

        // search for missing media file in media directory and show directory
        if( !wxFileName(media_file).Exists() || !wxFileName(media_file).IsFileReadable() )
        {
            wxFileName detect_media(selected_file);
            detect_media.SetExt("mp3");

            // search media directory
            detect_media.SetPath(mediaDirectory);
            if( detect_media.FileExists() )
            {
                media_file = detect_media;
                ObtainAccessToURL(media_file.GetFullPath().ToStdString());
            }
            else
            {
                // search selected file directory
                detect_media.SetPath(selected_file.GetPath());
                if( detect_media.FileExists() )
                {
                    media_file = detect_media;
                    ObtainAccessToURL(media_file.GetFullPath().ToStdString());
                }
            }
        }

        // if fseq or xseq had media update xml
        if( !CurrentSeqXmlFile->HasAudioMedia()
           && wxFileName(media_file).Exists()
           && wxFileName(media_file).IsFileReadable())
        {
            CurrentSeqXmlFile->SetMediaFile(GetShowDirectory(), media_file.GetFullPath(), true);
            int length_ms = CurrentSeqXmlFile->GetMedia()->LengthMS();
            CurrentSeqXmlFile->SetSequenceDurationMS(length_ms);
            playAnimation = false;
        }
        else
        {
            playAnimation = true;
        }

        if( CurrentSeqXmlFile->WasConverted() )
        {
            SeqSettingsDialog setting_dlg(this, CurrentSeqXmlFile, mediaDirectory, wxT("V3 file was converted. Please check settings!"));
			setting_dlg.Fit();
            int ret_code = setting_dlg.ShowModal();

            if (ret_code == NEEDS_RENDER)
            {
                RenderAll();
            }

			if (CurrentSeqXmlFile->GetMedia()->GetFrameInterval() < 0)
			{
				CurrentSeqXmlFile->GetMedia()->SetFrameInterval(CurrentSeqXmlFile->GetSequenceTimingAsInt());
			}
			SetAudioControls();
		}

        wxString mss = CurrentSeqXmlFile->GetSequenceTiming();
        int ms = atoi(mss.c_str());
        loaded_xml = SeqLoadXlightsFile(*CurrentSeqXmlFile, true);

        unsigned int numChan = GetMaxNumChannels();
        if( (numChan > SeqData.NumChannels()) ||
            (CurrentSeqXmlFile->GetSequenceDurationMS() / ms) > SeqData.NumFrames() )
        {
            SeqData.init(GetMaxNumChannels(), mMediaLengthMS / ms, ms);
        }
        else if( !loaded_fseq )
        {
            SeqData.init(GetMaxNumChannels(), CurrentSeqXmlFile->GetSequenceDurationMS() / ms, ms);
        }
        displayElementsPanel->Initialize();
        Timer1.Start(SeqData.FrameTime());

        if( loaded_fseq )
        {
            UpdatePreview();
        }
        else if( !loaded_xml )
        {
            SetStatusText(wxString::Format("Failed to load: '%s'.", filename));
            return;
        }

        float elapsedTime = sw.Time()/1000.0; //msec => sec
        SetStatusText(wxString::Format("'%s' loaded in %4.3f sec.", filename, elapsedTime));
        SetTitle(xlights_base_name + " - " + filename);

        EnableSequenceControls(true);
        Notebook1->SetSelection(Notebook1->GetPageIndex(PanelSequencer));
    }
}

bool xLightsFrame::CloseSequence()
{
    if( mSavedChangeCount !=  mSequenceElements.GetChangeCount() )
    {
        SaveChangesDialog* dlg = new SaveChangesDialog(this);
        if( dlg->ShowModal() == wxID_CANCEL )
        {
            return false;
        }
        if( dlg->GetSaveChanges() )
        {
            SaveSequence();
            //must wait for the rendering to complete
            while (renderProgressInfo != nullptr) {
                wxMilliSleep(10);
                wxYield();
            }
        }
    }

    // clear everything to prepare for new sequence
    sEffectAssist->SetPanel(nullptr);
    xlightsFilename = "";
    mediaFilename.Clear();
    previewLoaded = false;
    previewPlaying = false;
    ResetTimer(NO_SEQ);
    playType = 0;
    selectedEffect = NULL;
    if( CurrentSeqXmlFile )
    {
        delete CurrentSeqXmlFile;
        CurrentSeqXmlFile = NULL;
    }
    mSequenceElements.Clear();
    mSavedChangeCount = mSequenceElements.GetChangeCount();
    mLastAutosaveCount = mSavedChangeCount;

    mainSequencer->PanelWaveForm->CloseMedia();
    SeqData.init(0,0,50);
    EnableSequenceControls(true);  // let it re-evaluate menu state
    SetStatusText("");
    SetStatusText(CurrentDir, 1);
    sPreview1->Refresh();
    sPreview2->Refresh();

    SetTitle( xlights_base_name + xlights_qualifier + " (Ver " + xlights_version_string + ") " + xlights_build_date );

    return true;
}

bool xLightsFrame::SeqLoadXlightsFile(const wxString& filename, bool ChooseModels)
{
    delete xLightsFrame::CurrentSeqXmlFile;
    xLightsFrame::CurrentSeqXmlFile = new xLightsXmlFile(filename);
    return SeqLoadXlightsFile(*xLightsFrame::CurrentSeqXmlFile, ChooseModels);
}

// Load the xml file containing effects for a particular sequence
// Returns true if file exists and was read successfully
bool xLightsFrame::SeqLoadXlightsFile(xLightsXmlFile& xml_file, bool ChooseModels )
{
    if( xml_file.IsOpen() )
    {
        LoadSequencer(xml_file);
        xml_file.SetSequenceLoaded(true);
        return true;
    }

    return false;
}

void xLightsFrame::ClearSequenceData()
{
    wxASSERT(SeqData.IsValidData());
    for (size_t i = 0; i < SeqData.NumFrames(); ++i)
        SeqData[i].Zero();
}

void xLightsFrame::RenderIseqData(bool bottom_layers, ConvertLogDialog* plog)
{
    DataLayerSet& data_layers = CurrentSeqXmlFile->GetDataLayers();
    ConvertParameters::ReadMode read_mode;
    if (bottom_layers && data_layers.GetNumLayers() == 1 &&
        data_layers.GetDataLayer(0)->GetName() == "Nutcracker") {
        DataLayer* nut_layer = data_layers.GetDataLayer(0);
        if( nut_layer->GetDataSource() == xLightsXmlFile::CANVAS_MODE ) {
            //Don't clear, v3 workflow of augmenting the existing fseq file
            return;
        }
    }

    if( bottom_layers )
    {
        ClearSequenceData();
        read_mode = ConvertParameters::READ_MODE_NORMAL;
    }
    else
    {
        read_mode = ConvertParameters::READ_MODE_IGNORE_BLACK;
    }
    int layers_rendered = 0;
    bool start_rendering = bottom_layers;
    for( int i = data_layers.GetNumLayers() - 1; i >= 0; --i )  // build layers bottom up
    {
        DataLayer* data_layer = data_layers.GetDataLayer(i);

        if( data_layer->GetName() != "Nutcracker" )
        {
            if( start_rendering )
            {
                if (plog != NULL)
                {
                    plog->Show(true);
                }
                ConvertParameters read_params(data_layer->GetDataSource(),                // input filename
                                              SeqData,                                    // sequence data object
                                              GetNetInfo(),                               // global network info
                                              read_mode,                                  // file read mode
                                              this,                                       // xLights main frame
                                              nullptr,
                                              plog,
                                              nullptr,                                    // filename not needed
                                              data_layer );                               // provide data layer for channel offsets

                FileConverter::ReadFalconFile(read_params);
                read_mode = ConvertParameters::READ_MODE_IGNORE_BLACK;
                layers_rendered++;
            }
        }
        else
        {
            if( bottom_layers ) break;  // exit after Nutcracker layer if rendering bottom layers only
            start_rendering = true;
        }
    }
}

void xLightsFrame::SetSequenceEnd(int ms)
{
    mainSequencer->PanelTimeLine->SetSequenceEnd(CurrentSeqXmlFile->GetSequenceDurationMS());
    mSequenceElements.SetSequenceEnd(CurrentSeqXmlFile->GetSequenceDurationMS());
}

static bool CalcPercentage(std::string& value, double base, bool reverse, int offset)
{
    int val = wxAtoi(value);
    val -= offset;
    val %= (int)base;
    if( val < 0 ) return false;
    double half_width = 1.0/base*50.0;
    double percent = (double)val/base*100.0 + half_width;
    if( reverse )
    {
        percent = 100.0 - percent;
    }
    value = wxString::Format("%d",(int)percent);
    return true;
}
static bool CalcBoundedPercentage(std::string& value, int base, bool reverse, int offset) {
    int val = wxAtoi(value);
    val -= offset;
    val %= (int)base;
    if( val < 0 ) return false;
    if (val == 0) {
        value = reverse ? "99.9" : "0.0";
    } else if (val == (base - 1)) {
        value = reverse ? "0.0" : "99.9";
    } else {
        return CalcPercentage(value, base, reverse, offset);
    }
    return true;
}
static int CalcUnBoundedPercentage(int val, int base) {
    double half_width = 50.0/base;
    double percent = (double)val/base*100.0 + half_width;

    return percent;
}
static xlColor GetColor(const std::string& sRed, const std::string& sGreen, const std::string& sBlue)
{
    double red,green,blue;
    red = atof(sRed.c_str());
    red = red / 100.0 * 255.0;
    green = atof(sGreen.c_str());
    green = green / 100.0 * 255.0;
    blue = atof(sBlue.c_str());
    blue = blue / 100.0 * 255.0;
    xlColor color(red, green, blue);
    return color;
}
static wxString GetColorString(const std::string& sRed, const std::string& sGreen, const std::string& sBlue)
{
    return GetColor(sRed, sGreen, sBlue);
}
static xlColor GetColor(const std::string& rgb) {
    int i = wxAtoi(rgb);
    xlColor cl;
    cl.red = (i & 0xff);
    cl.green = ((i >> 8) & 0xFF);
    cl.blue = ((i >> 16) & 0xff);
    return cl;
}

static EffectLayer* FindOpenLayer(Element* model, int layer_index, int startTimeMS, int endTimeMS, std::vector<bool> &reserved)
{
    EffectLayer* layer;
    int index = layer_index-1;

    layer = model->GetEffectLayer(index);
    if (layer != nullptr && layer->GetRangeIsClearMS(startTimeMS, endTimeMS) )
    {
        return layer;
    }

    // need to search for open layer
    for( size_t i = 0; i < model->GetEffectLayerCount(); i++ )
    {
        if (i >= reserved.size() || !reserved[i]) {
            layer = model->GetEffectLayer(i);
            if( layer->GetRangeIsClearMS(startTimeMS, endTimeMS) )
            {
                return layer;
            }
        }
    }

    // empty layer not found so create a new one
    layer = model->AddEffectLayer();
    if (model->GetEffectLayerCount() > reserved.size()) {
        reserved.resize(model->GetEffectLayerCount(), false);
    }
    return layer;
}

#define MAXBUFSIZE 4096
class FixXMLInputStream : public wxInputStream {
public:
    FixXMLInputStream(wxInputStream & in) : wxInputStream(), bin(in) {
    }
    void fillBuf() {
        int pos = bufLen;
        int sz =  MAXBUFSIZE - bufLen;
        bin.Read(&buf[pos], sz);
        size_t ret = bin.LastRead();
        bufLen += ret;

        bool needToClose = false;
        for (size_t x = 7; x < bufLen; x++) {
            if (buf[x-7] == '<'
                && buf[x-6] == 'p'
                && buf[x-5] == 'i'
                && buf[x-4] == 'x'
                && buf[x-3] == 'e'
                && buf[x-2] == 'l'
                && buf[x-1] == 's'
                && buf[x] == '=') {
                buf[x-2] = ' ';
            } else if (buf[x-7] == '<'
                       && buf[x-6] == 't'
                       && buf[x-5] == 'i'
                       && buf[x-4] == 'm'
                       && buf[x-3] == 'i'
                       && buf[x-2] == 'n'
                       && buf[x-1] == 'g'
                       && buf[x] == ' ') {
                needToClose = true;
            } else if (x > 6 &&
                       buf[x-6] == '<'
                       && buf[x-5] == 'g'
                       && buf[x-4] == 'r'
                       && buf[x-3] == 'o'
                       && buf[x-2] == 'u'
                       && buf[x-1] == 'p'
                       && buf[x] == ' ') {
                needToClose = true;
            } else if (x > 12 &&
                       buf[x-12] == '<'
                       && buf[x-11] == 'i'
                       && buf[x-10] == 'm'
                       && buf[x-9] == 'a'
                       && buf[x-8] == 'g'
                       && buf[x-7] == 'e'
                       && buf[x-6] == 'A'
                       && buf[x-5] == 'c'
                       && buf[x-4] == 't'
                       && buf[x-3] == 'i'
                       && buf[x-2] == 'o'
                       && buf[x-1] == 'n'
                       && buf[x] == ' ') {
                needToClose = true;
            } else if (x > 11 &&
                       buf[x-11] == '<'
                       && buf[x-10] == 't'
                       && buf[x-9] == 'e'
                       && buf[x-8] == 'x'
                       && buf[x-7] == 't'
                       && buf[x-6] == 'A'
                       && buf[x-5] == 'c'
                       && buf[x-4] == 't'
                       && buf[x-3] == 'i'
                       && buf[x-2] == 'o'
                       && buf[x-1] == 'n'
                       && buf[x] == ' ') {
                needToClose = true;
            } else if (x > 14 &&
                       buf[x-14] == '<'
                       && buf[x-13] == 'c'
                       && buf[x-12] == 'o'
                       && buf[x-11] == 'n'
                       && buf[x-10] == 'f'
                       && buf[x-9] == 'i'
                       && buf[x-8] == 'g'
                       && buf[x-7] == 'u'
                       && buf[x-6] == 'r'
                       && buf[x-5] == 'a'
                       && buf[x-4] == 't'
                       && buf[x-3] == 'i'
                       && buf[x-2] == 'o'
                       && buf[x-1] == 'n'
                       && buf[x] == ' ') {
                needToClose = true;
            } else if (x > 15 &&
                       buf[x-15] == '<'
                       && buf[x-14] == '/'
                       && buf[x-13] == 'c'
                       && buf[x-12] == 'o'
                       && buf[x-11] == 'n'
                       && buf[x-10] == 'f'
                       && buf[x-9] == 'i'
                       && buf[x-8] == 'g'
                       && buf[x-7] == 'u'
                       && buf[x-6] == 'r'
                       && buf[x-5] == 'a'
                       && buf[x-4] == 't'
                       && buf[x-3] == 'i'
                       && buf[x-2] == 'o'
                       && buf[x-1] == 'n'
                       && buf[x] == '>') {
                for (int y = x - 15; y <= x; y++) {
                    buf[y] = ' ';
                }
            } else if (buf[x-1] == '>' && needToClose) {
                if (buf[x-2] != '/') {
                    buf[x - 1] = '/';
                    buf[x] = '>';
                }
                needToClose = false;
            }
        }
    }

    virtual size_t OnSysRead(void *buffer, size_t bufsize) {
        unsigned char *b = (unsigned char *)buffer;
        if (bufsize > 1024) {
            bufsize = 1024;
        }

        if (bufLen < 2000) {
            fillBuf();
        }

        if (bufLen) {
            size_t ret = std::min(bufsize, bufLen);
            memcpy(b, buf, ret);
            for (int x = ret; x < bufLen; x++) {
                buf[x-ret] = buf[x];
            }
            bufLen -= ret;
            buf[bufLen] = 0;
            return ret;
        }
        return 0;
    }

private:
    wxBufferedInputStream bin;
    unsigned char buf[MAXBUFSIZE];
    size_t bufLen = 0;
};


void xLightsFrame::OnMenuItemImportEffects(wxCommandEvent& event)
{
    wxArrayString filters;
    
    filters.push_back("SuperStar File (*.sup)|*.sup");
    filters.push_back("LOR Music Sequences (*.lms)|*.lms");
        filters.push_back("xLights Sequence (*.xml)|*.xml");
            filters.push_back("HLS hlsIdata Sequences(*.hlsIdata)|*.hlsIdata");
                filters.push_back("Vixen 2.x Sequence(*.vix)|*.vix");
                    filters.push_back("LSP 2.x Sequence(*.msq)|*.msq");

                    wxString filter;
    for (auto it = filters.begin(); it != filters.end(); ++it)
    {
        if (filter != "")
        {
            filter += "|";
        }
        filter += *it;
    }

    wxFileDialog file(this, "Choose file to import", "", "", filter);

    wxString lit = "";
    wxConfigBase* config = wxConfigBase::Get();
    if (config != nullptr)
    {
        config->Read("xLightsLastImportType", &lit, "");
    }
    if (lit != "")
    {
        int index = 0;

        for (auto it = filters.begin(); it != filters.end(); ++it)
        {
            if (lit == *it)
            {
                file.SetFilterIndex(index);
                break;
            }
            index++;
        }
    }

    if (file.ShowModal() == wxID_OK) {

        if (config != nullptr)
        {
            config->Write("xLightsLastImportType", filters[file.GetFilterIndex()]);
        }

        wxFileName fn = file.GetPath();
        if (!fn.Exists()) {
            return;
        }
        wxString ext = fn.GetExt().Lower();
        if (ext == "lms") {
            ImportLMS(fn);
        } else if (ext == "hlsidata") {
            ImportHLS(fn);
        } else if (ext == "sup") {
            ImportSuperStar(fn);
        } else if (ext == "vix") {
            ImportVix(fn);
        } else if (ext == "xml") {
            ImportXLights(fn);
        } else if (ext == "msq") {
            ImportLSP(fn);
        }
        wxCommandEvent eventRowHeaderChanged(EVT_ROW_HEADINGS_CHANGED);
        wxPostEvent(this, eventRowHeaderChanged);
        mainSequencer->PanelEffectGrid->Refresh();
    }
}

void MapXLightsEffects(EffectLayer *target, EffectLayer *src, std::vector<EffectLayer *> &mapped) {
    for (int x = 0; x < src->GetEffectCount(); x++) {
        Effect *ef = src->GetEffect(x);
        target->AddEffect(0, ef->GetEffectName(), ef->GetSettingsAsString(), ef->GetPaletteAsString(),
                          ef->GetStartTimeMS(), ef->GetEndTimeMS(), 0, 0);
    }
    mapped.push_back(src);
}
void MapXLightsStrandEffects(EffectLayer *target, const std::string &name,
                             std::map<std::string, EffectLayer *> &layerMap,
                             SequenceElements &seqEl,
                             std::vector<EffectLayer *> &mapped) {
    EffectLayer *src = layerMap[name];
    if (src == nullptr) {
        Element * srcEl = seqEl.GetElement(name);
        if (srcEl == nullptr) {
            printf("Source strand %s doesn't exist\n", name.c_str());
            return;
        }
        src = srcEl->GetEffectLayer(0);
    }
    if (src != nullptr) {
        MapXLightsEffects(target, src, mapped);
    } else {
        printf("Source strand %s doesn't exist\n", name.c_str());
    }
}
void MapXLightsEffects(Element *target,
                       const std::string &name,
                       SequenceElements &seqEl,
                       std::map<std::string, Element *> &elementMap,
                       std::map<std::string, EffectLayer *> &layerMap,
                       std::vector<EffectLayer *> &mapped) {
    EffectLayer *src = layerMap[name];
    Element *el = elementMap[name];
    if (src != nullptr) {
        MapXLightsEffects(target->GetEffectLayer(0), src, mapped);
        return;
    }
    if (el == nullptr) {
        el = seqEl.GetElement(name);
    }
    if (el == nullptr) {
        //printf("Source element %s doesn't exist\n", name.c_str());
        return;
    }
    while (target->GetEffectLayerCount() < el->GetEffectLayerCount()) {
        target->AddEffectLayer();
    }
    for (size_t x = 0; x < el->GetEffectLayerCount(); x++) {
        MapXLightsEffects(target->GetEffectLayer(x), el->GetEffectLayer(x), mapped);
    }
}

void xLightsFrame::ImportXLights(const wxFileName &filename) {
    wxStopWatch sw; // start a stopwatch timer

    xLightsXmlFile xlf(filename);
    xlf.Open(GetShowDirectory(), true);
    SequenceElements se(this);
    se.SetFrequency(mSequenceElements.GetFrequency());
    se.SetViewsNode(ViewsNode); // This must come first before LoadSequencerFile.
    se.LoadSequencerFile(xlf, GetShowDirectory());
    xlf.AdjustEffectSettingsForVersion(se, this);

    std::vector<Element *> elements;
    for (size_t e = 0; e < se.GetElementCount(); e++) {
        Element *el = se.GetElement(e);
        elements.push_back(el);
    }
    ImportXLights(se, elements, filename);

    float elapsedTime = sw.Time()/1000.0; //msec => sec
    SetStatusText(wxString::Format("'%s' imported in %4.3f sec.", filename.GetPath(), elapsedTime));
}

ModelElement * AddModel(Model *m, SequenceElements &se) {
    se.AddMissingModelsToSequence(m->GetName(), false);
    ModelElement * model = dynamic_cast<ModelElement*>(se.GetElement(m->GetName()));
    model->Init(*m);
    return model;
}
void xLightsFrame::ImportXLights(SequenceElements &se, const std::vector<Element *> &elements, const wxFileName &filename,
                                 bool allowAllModels, bool clearSrc) {
    std::map<std::string, EffectLayer *> layerMap;
    std::map<std::string, Element *>elementMap;
    xLightsImportChannelMapDialog dlg(this, filename);
    dlg.mSequenceElements = &mSequenceElements;
    dlg.xlights = this;
    std::vector<EffectLayer *> mapped;
    std::vector<std::string> timingTrackNames;
    std::map<std::string, TimingElement*> timingTracks;

    for (auto it = elements.begin(); it != elements.end(); it++) {
        Element *e = *it;
        if (e->GetType() == ELEMENT_TYPE_MODEL)
        {
            ModelElement *el = dynamic_cast<ModelElement*>(e);
            bool hasEffects = false;
            for (size_t l = 0; l < el->GetEffectLayerCount(); l++) {
                hasEffects |= el->GetEffectLayer(l)->GetEffectCount() > 0;
            }
            if (hasEffects) {
                dlg.channelNames.push_back(el->GetName());
            }
            elementMap[el->GetName()] = el;
            int s = 0;
            for (size_t sm = 0; sm < el->GetSubModelCount(); sm++) {
                SubModelElement *sme = el->GetSubModel(sm);

                StrandElement *ste = dynamic_cast<StrandElement *>(sme);
                std::string smName = sme->GetName();
                if (ste != nullptr) {
                    s++;
                    if (smName == "") {
                        smName = wxString::Format("Strand %d", s);
                    }
                }
                if (sme->HasEffects()) {
                    elementMap[el->GetName() + "/" + smName] = sme;
                    dlg.channelNames.push_back(el->GetName() + "/" + smName);
                }
                if (ste != nullptr) {
                    for (size_t n = 0; n < ste->GetNodeLayerCount(); n++) {
                        NodeLayer *nl = ste->GetNodeLayer(n, true);
                        if (nl->GetEffectCount() > 0) {
                            std::string nodeName = nl->GetName();
                            if (nodeName == "") {
                                nodeName = wxString::Format("Node %d", (n + 1));
                            }
                            dlg.channelNames.push_back(el->GetName() + "/" + smName + "/" + nodeName);
                            layerMap[el->GetName() + "/" + smName + "/" + nodeName] = nl;
                        }
                    }
                }
            }
        } else if (e->GetType() == ELEMENT_TYPE_TIMING) {
            TimingElement *tel = dynamic_cast<TimingElement*>(e);
            if (tel->GetFixedTiming() == 0) {
                bool hasEffects = false;
                for (size_t n = 0; n < tel->GetEffectLayerCount(); n++) {
                    hasEffects |= tel->GetEffectLayer(n)->GetEffectCount() > 0;
                }
                if (hasEffects) {
                    timingTrackNames.push_back(tel->GetName());
                    timingTracks[tel->GetName()] = tel;
                }
            }
        }
    }

    std::sort(dlg.channelNames.begin(), dlg.channelNames.end());
    dlg.channelNames.insert(dlg.channelNames.begin(), "");
    dlg.timingTracks = timingTrackNames;
    bool ok = dlg.Init();

    if (!ok || dlg.ShowModal() != wxID_OK) {
        return;
    }
    
    for (int tt = 0; tt < dlg.TimingTrackListBox->GetCount(); tt++) {
        if (dlg.TimingTrackListBox->IsChecked(tt)) {
            TimingElement *tel = timingTracks[timingTrackNames[tt]];
            TimingElement *target = (TimingElement*)mSequenceElements.AddElement(tel->GetName(), "timing", true, tel->GetCollapsed(), tel->GetActive(), false);
            char cnt = '1';
            while (target == nullptr) {
                target = (TimingElement*)mSequenceElements.AddElement(tel->GetName() + "-" + cnt++, "timing", true, tel->GetCollapsed(), tel->GetActive(), false);
            }
            for (int l = 0; l < tel->GetEffectLayerCount(); l++) {
                EffectLayer *src = tel->GetEffectLayer(l);
                while (l >= target->GetEffectLayerCount()) {
                    target->AddEffectLayer();
                }
                EffectLayer *dst = target->GetEffectLayer(l);
                std::vector<EffectLayer *> mapped;
                MapXLightsEffects(dst, src, mapped);
            }
        }
    }
    
    for (size_t i = 0; i < dlg.dataModel->GetChildCount(); i++)
    {
        xLightsImportModelNode* m = dlg.dataModel->GetNthChild(i);
        std::string modelName = m->_model.ToStdString();
        ModelElement * model = nullptr;
        for (size_t x=0; x<mSequenceElements.GetElementCount();x++) {
            if (mSequenceElements.GetElement(x)->GetType() == ELEMENT_TYPE_MODEL
                && modelName == mSequenceElements.GetElement(x)->GetName()) {
                model = dynamic_cast<ModelElement*>(mSequenceElements.GetElement(x));
                break;
            }
        }
        if (m->_mapping != "") {
            if (model == nullptr) {
                model = AddModel(GetModel(modelName), mSequenceElements);
            }
            MapXLightsEffects(model, m->_mapping.ToStdString(), se, elementMap, layerMap, mapped);
        }

        int str = 0;
        for (size_t j = 0; j < m->GetChildCount(); j++)
        {
            xLightsImportModelNode* s = m->GetNthChild(j);

            if ("" != s->_mapping) {
                if (model == nullptr) {
                    model = AddModel(GetModel(modelName), mSequenceElements);
                }
                SubModelElement *ste =  model->GetSubModel(str);
                if( ste != nullptr ) {
                    MapXLightsEffects(ste, s->_mapping.ToStdString(), se, elementMap, layerMap, mapped);
                }
            }
            for (size_t n = 0; n < s->GetChildCount(); n++) {
                xLightsImportModelNode* ns = s->GetNthChild(n);
                if ("" != ns->_mapping) {
                    if (model == nullptr) {
                        model = AddModel(GetModel(modelName), mSequenceElements);
                    }
                    SubModelElement *ste =  model->GetSubModel(str);
                    StrandElement *stre = dynamic_cast<StrandElement *>(ste);
                    if (stre != nullptr) {
                        NodeLayer *nl = stre->GetNodeLayer(n, true);
                        if (nl != nullptr) {
                            MapXLightsStrandEffects(nl, ns->_mapping.ToStdString(), layerMap, se, mapped);
                        }
                    }
                }
            }
            str++;
        }
    }
    if (clearSrc) {
        for (auto it = mapped.begin(); it != mapped.end(); it++) {
            (*it)->RemoveAllEffects();
        }
    }
}

void MapToStrandName(const std::string &name, std::vector<std::string> &strands) {
    if (name.find("_") != name.npos) {
        int idx = name.find("_") + 1;
        //maybe map to a strand?  Name_0001, Name_0002... etc...

        int ppos = -1;
        int spos = -1;
        for (int x = idx; x < name.size(); x++) {
            if (name[x] == 'P') {
                ppos = x;
            } else if (name[x] == 'S') {
                spos = x;
            } else if (name[x] < '0' || name[x] > '9') {
                return;
            }
        }
        std::string strandName;
        if (spos == -1 && ppos == -1) {
            //simple "strand" names of _#####
            strandName = name.substr(0, name.find("_") - 1);
        } else if (spos >= 0 && ppos > spos) {
            //more complex of _S###P###
            strandName = name.substr(0, ppos - 1);
        }
        if ("" != strandName && std::find(strands.begin(), strands.end(), strandName) == strands.end()) {
            strands.push_back(strandName);
        }
    }
}
void ReadHLSData(wxXmlNode *chand, std::vector<unsigned char> & data) {
    for (wxXmlNode* chani=chand->GetChildren(); chani!=NULL; chani=chani->GetNext()) {
        if ("IlluminationData" == chani->GetName()) {
            for (wxXmlNode* block=chani->GetChildren(); block!=NULL; block=block->GetNext()) {
                wxString vals = block->GetChildren()->GetContent();
                int offset = wxAtoi(vals.SubString(0, vals.Find("-")));
                vals = vals.SubString(vals.Find("-")+1, vals.size());
                while (!vals.IsEmpty()) {
                    wxString v = vals.BeforeFirst(',');
                    vals = vals.AfterFirst(',');
                    long iv = 0;
                    v.ToLong(&iv, 16);
                    data[offset] = iv;
                    offset++;
                }
            }
        }
    }
}
void MapHLSChannelInformation(xLightsFrame *xlights, EffectLayer *layer, wxXmlNode* tuniv, int frames, int frameTime,
                              const wxString &cn, wxColor color, Model &mc, bool byStrand) {
    if (cn == "") {
        return;
    }
    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    wxXmlNode *redNode = nullptr;
    wxXmlNode *greenNode = nullptr;
    wxXmlNode *blueNode = nullptr;

    for (wxXmlNode* univ=tuniv->GetChildren(); univ!=NULL; univ=univ->GetNext()) {
        if (univ->GetName() == "Universe") {
            for (wxXmlNode* channels=univ->GetChildren(); channels!=NULL; channels=channels->GetNext()) {
                if (channels->GetName() == "Channels") {
                    for (wxXmlNode* chand=channels->GetChildren(); chand!=NULL; chand=chand->GetNext()) {
                        if (chand->GetName() == "ChannelData") {
                            for (wxXmlNode* chani=chand->GetChildren(); chani!=NULL; chani=chani->GetNext()) {
                                if (chani->GetName() == "ChanInfo") {
                                    wxString info = chani->GetChildren()->GetContent();
                                    if (info == cn + ", Normal") {
                                        //single channel, easy
                                        redNode = chand;
                                    } else if (info == cn + ", RGB-R") {
                                        redNode = chand;
                                    } else if (info == cn + ", RGB-G") {
                                        greenNode = chand;
                                    } else if (info == cn + ", RGB-B") {
                                        blueNode = chand;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (redNode == nullptr) {
        printf("Did not map %s\n", (const char *)cn.c_str());
		logger_base.info("Did not map " + cn);
		return;
    }
    std::vector<unsigned char> redData(frames);
    std::vector<unsigned char> greenData(frames);
    std::vector<unsigned char> blueData(frames);
    std::vector<xlColor> colors(frames);
    ReadHLSData(redNode, redData);
    if (greenNode != nullptr) {
        ReadHLSData(greenNode, greenData);
        ReadHLSData(blueNode, blueData);
        for (int x = 0; x < frames; x++) {
            colors[x].Set(redData[x], greenData[x], blueData[x]);
        }
    } else {
        xlColor c(color.Red(), color.Green(), color.Blue());
        HSVValue hsv = c.asHSV();
        for (int x = 0; x < frames; x++) {
            int i = redData[x];
            //for ramps up/down, HLS does a 1%-100% so the first cell is not linear and
            //we end up not able to map the ramps, we'll try and detect that here
            //and change to 0
            if (i <= 3 && i > 0) {
                if (x < (frames-4)) {
                    if (i < redData[x + 1] && redData[x + 1] < redData[x + 2] && redData[x + 2] < redData[x + 3]) {
                        i = 0;
                    }
                }
                if (x > 4) {
                    if (i < redData[x - 1] && redData[x - 1] < redData[x - 2] && redData[x - 2] < redData[x - 3]) {
                        i = 0;
                    }
                }
            }
            hsv.value = ((double)i) / 255.0;
            colors[x] = hsv;
        }
    }
    xlights->ConvertDataRowToEffects(layer, colors, frameTime);
}
std::string FindHLSStrandName(const std::string &ccrName, int node, const std::vector<std::string> &channelNames) {
    std::string r = ccrName + wxString::Format("P%03d", node).ToStdString();
    if (std::find(channelNames.begin(), channelNames.end(), r) == channelNames.end()) {
        r = ccrName + wxString::Format("P%04d", node);
    } else {
        return r;
    }
    if (std::find(channelNames.begin(), channelNames.end(), r) == channelNames.end()) {
        r = ccrName + wxString::Format("P%02d", node);
    } else {
        return r;
    }
    if (std::find(channelNames.begin(), channelNames.end(), r) == channelNames.end()) {
        r = ccrName + wxString::Format("_%04d", node);
    } else {
        return r;
    }
    if (std::find(channelNames.begin(), channelNames.end(), r) == channelNames.end()) {
        r = ccrName + wxString::Format("_%03d", node);
    } else {
        return r;
    }
    if (std::find(channelNames.begin(), channelNames.end(), r) == channelNames.end()) {
        return r;
    }
    return "";
}
int base64_decode(const wxString& encoded_string, std::vector<unsigned char> &data);


bool EndsWith(const std::string &fullString, const std::string &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    }
    return false;
}
bool Contains(const std::vector<std::string> &array, const std::string &str) {
    return std::find(array.begin(), array.end(), str) != array.end();
}
int Index(const std::vector<std::string> &array, const std::string &str) {
    auto it = std::find(array.begin(), array.end(), str);
    if (it == array.end()) {
        return -1;
    }
    return it - array.begin();
}
void MapVixChannelInformation(xLightsFrame *xlights, EffectLayer *layer,
                              std::vector<unsigned char> &data,
                              int frameTime,
                              int numFrames,
                              const std::string & channelName,
                              const std::vector<std::string> &channels,
                              wxColor color,
                              Model &mc) {
    if (channelName == "") {
        return;
    }
    int channel = Index(channels, channelName);
    xlColorVector colors(numFrames);
    if (channel == wxNOT_FOUND) {
        int rchannel = Index(channels, channelName + "Red");
        if (rchannel == wxNOT_FOUND) {
            rchannel = Index(channels, channelName + "-R");
        }
        int gchannel = Index(channels, channelName + "Green");
        if (gchannel == wxNOT_FOUND) {
            gchannel = Index(channels, channelName + "-G");
        }
        int bchannel = Index(channels, channelName + "Blue");
        if (bchannel == wxNOT_FOUND) {
            bchannel = Index(channels, channelName + "-B");
        }
        if (rchannel == wxNOT_FOUND || gchannel == wxNOT_FOUND || bchannel == wxNOT_FOUND) {
            return;
        }
        for (int x = 0; x < numFrames; x++) {
            colors[x].Set(data[x + numFrames * rchannel], data[x + numFrames * gchannel], data[x + numFrames * bchannel]);
        }
    } else {
        xlColor c(color.Red(), color.Green(), color.Blue());
        HSVValue hsv = c.asHSV();
        for (int x = 0; x < numFrames; x++) {
            hsv.value = ((double)data[x + numFrames * channel]) / 255.0;
            colors[x] = hsv;
        }
    }
    xlights->ConvertDataRowToEffects(layer, colors, frameTime);
}

// xml
#include "../include/spxml-0.5/spxmlparser.hpp"
#include "../include/spxml-0.5/spxmlevent.hpp"
#ifndef MAX_READ_BLOCK_SIZE
#define MAX_READ_BLOCK_SIZE 4096 * 1024
#endif


void xLightsFrame::ImportVix(const wxFileName &filename) {
    wxStopWatch sw; // start a stopwatch timer

    std::string NodeName,NodeValue,msg;
    std::vector<unsigned char> VixSeqData;
    long cnt = 0;
    wxArrayString context;
    long MaxIntensity = 255;

    int time = 0;
    int frameTime = 50;


    LMSImportChannelMapDialog dlg(this, filename);
    dlg.mSequenceElements = &mSequenceElements;
    dlg.xlights = this;


    SP_XmlPullParser *parser = new SP_XmlPullParser();
    parser->setMaxTextSize(MAX_READ_BLOCK_SIZE / 2);
    wxFile file(filename.GetFullPath());
    char *bytes = new char[MAX_READ_BLOCK_SIZE];
    size_t read = file.Read(bytes, MAX_READ_BLOCK_SIZE);
    parser->append(bytes, read);
    wxString carryOver;

    std::vector<std::string> unsortedChannels;

    int chanColor = -1;

    //pass 1, read the length, determine number of networks, units/network, channels per unit
    SP_XmlPullEvent * event = parser->getNext();
    int done = 0;
    while (!done) {
        if (!event) {
            read = file.Read(bytes, MAX_READ_BLOCK_SIZE);
            if (read == 0) {
                done = true;
            } else {
                parser->append(bytes, read);
            }
        } else {
            switch(event -> getEventType()) {
                case SP_XmlPullEvent::eEndDocument:
                    done = true;
                    break;
                case SP_XmlPullEvent::eStartTag:
                {
                    SP_XmlStartTagEvent * stagEvent = (SP_XmlStartTagEvent*)event;
                    NodeName = stagEvent->getName();
                    context.push_back(NodeName);
                    cnt++;
                    if (cnt > 1 && context[1] == wxString("Channels") && NodeName == wxString("Channel")) {
                        chanColor = wxAtoi(stagEvent -> getAttrValue("color")) & 0xFFFFFF;
                    }
                }
                break;
                case SP_XmlPullEvent::eCData:
                {
                    SP_XmlCDataEvent * stagEvent = (SP_XmlCDataEvent*)event;
                    if (cnt >= 2) {
                        NodeValue = stagEvent->getText();
                        if (context[1] == wxString("MaximumLevel")) {
                            MaxIntensity = wxAtoi(NodeValue);
                        } else if (context[1] == wxString("EventPeriodInMilliseconds")) {
                            frameTime = wxAtoi(NodeValue);
                        } else if (context[1] == wxString("Time")) {
                            time = wxAtoi(NodeValue);
                        } else if (context[1] == wxString("EventValues")) {
                            //AppendConvertStatus(string_format(wxString("Chunk Size=%d\n"), NodeValue.size()));
                            if (carryOver.size() > 0) {
                                NodeValue.insert(0, carryOver);
                            }
                            int i = base64_decode(NodeValue, VixSeqData);
                            if (i != 0) {
                                int start = NodeValue.size() - i - 1;
                                carryOver = NodeValue.substr(start, start + i);
                            } else {
                                carryOver.clear();
                            }
                        } else if (context[1] == wxString("Channels") && context[2] == wxString("Channel")) {
                            dlg.channelNames.push_back(NodeValue);
                            unsortedChannels.push_back(NodeValue);

                            xlColor c(chanColor, false);
                            bool addRGB = false;
                            std::string base;
                            if (EndsWith(NodeValue, "Red") || EndsWith(NodeValue, "-R")) {
                                c = xlRED;
                                if (EndsWith(NodeValue, "-R")) {
                                    base = NodeValue.substr(0, NodeValue.size() - 3);
                                } else {
                                    base = NodeValue.substr(0, NodeValue.size() - 4);
                                }
                                if ((Contains(dlg.channelNames, base + "Blue")
                                     && Contains(dlg.channelNames, base + "Green"))
                                    || (Contains(dlg.channelNames, base + "-B")
                                        && Contains(dlg.channelNames, base + "-G")))
                                {
                                    addRGB = true;
                                }
                            } else if (EndsWith(NodeValue, "Blue") || EndsWith(NodeValue, "-B")) {
                                c = xlBLUE;
                                if (EndsWith(NodeValue, "-B")) {
                                    base = NodeValue.substr(0, NodeValue.size() - 3);
                                } else {
                                    base = NodeValue.substr(0, NodeValue.size() - 5);
                                }
                                if ((Contains(dlg.channelNames, base + "Red")
                                     && Contains(dlg.channelNames, base + "Green"))
                                    || (Contains(dlg.channelNames, base + "-R")
                                        && Contains(dlg.channelNames, base + "-G"))) {
                                    addRGB = true;
                                }
                            } else if (EndsWith(NodeValue, "Green") || EndsWith(NodeValue, "-G")) {
                                c = xlGREEN;
                                if (EndsWith(NodeValue, "-G")) {
                                    base = NodeValue.substr(0, NodeValue.size() - 3);
                                } else {
                                    base = NodeValue.substr(0, NodeValue.size() - 6);
                                }
                                if ((Contains(dlg.channelNames, base + "Blue")
                                     && Contains(dlg.channelNames, base + "Red"))
                                    || (Contains(dlg.channelNames, base + "-B")
                                        && Contains(dlg.channelNames, base + "-R"))) {
                                    addRGB = true;
                                }
                            }
                            dlg.channelColors[NodeValue] = c;
                            if (addRGB) {
                                dlg.channelColors[base] = xlBLACK;
                                dlg.channelNames.push_back(base);
                            }
                        }
                    }
                    break;
                }
                case SP_XmlPullEvent::eEndTag:
                    if (cnt > 0) {
                        context.RemoveAt(cnt-1);
                    }
                    cnt = context.size();
                    break;
            }
            delete event;
        }
        if (!done) {
            event = parser->getNext();
        }
    }
    delete [] bytes;
    delete parser;
    file.Close();

    int numFrames = time / frameTime;

    std::sort(dlg.ccrNames.begin(), dlg.ccrNames.end());
    dlg.ccrNames.insert(dlg.ccrNames.begin(), "");

    std::sort(dlg.channelNames.begin(), dlg.channelNames.end());
    dlg.channelNames.insert(dlg.channelNames.begin(), "");

    dlg.MapByStrand->Hide();
    dlg.Init();

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    int row = 0;
    for (size_t m = 0; m < dlg.modelNames.size(); m++) {
        std::string modelName = dlg.modelNames[m];
        Model *mc = GetModel(modelName);
        ModelElement * model = nullptr;
        for (size_t i=0;i<mSequenceElements.GetElementCount();i++) {
            if (mSequenceElements.GetElement(i)->GetType() == ELEMENT_TYPE_MODEL
                && modelName == mSequenceElements.GetElement(i)->GetName()) {
                model = dynamic_cast<ModelElement*>(mSequenceElements.GetElement(i));
            }
        }
        MapVixChannelInformation(this, model->GetEffectLayer(0),
                                 VixSeqData, frameTime, numFrames,
                                 dlg.ChannelMapGrid->GetCellValue(row, 3).ToStdString(),
                                 unsortedChannels,
                                 dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                 *mc);
        row++;
        for (int str = 0; str < mc->GetNumSubModels(); str++) {
            SubModelElement *se =  model->GetSubModel(str);
            EffectLayer *sl = se->GetEffectLayer(0);

            if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                MapVixChannelInformation(this, sl,
                                         VixSeqData, frameTime, numFrames,
                                         dlg.ChannelMapGrid->GetCellValue(row, 3).ToStdString(),
                                         unsortedChannels,
                                         dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4), *mc);
            }
            row++;
        }
        for (int str = 0; str < mc->GetNumStrands(); str++) {
            StrandElement *se =  model->GetStrand(str, true);
            EffectLayer *sl = se->GetEffectLayer(0);

            if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                    MapVixChannelInformation(this, sl,
                                             VixSeqData, frameTime, numFrames,
                                             dlg.ChannelMapGrid->GetCellValue(row, 3).ToStdString(),
                                             unsortedChannels,
                                             dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4), *mc);
            }
            row++;
            for (int n = 0; n < mc->GetStrandLength(str); n++) {
                if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                    MapVixChannelInformation(this, se->GetNodeLayer(n, true),
                                             VixSeqData, frameTime, numFrames,
                                             dlg.ChannelMapGrid->GetCellValue(row, 3).ToStdString(),
                                             unsortedChannels,
                                             dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4), *mc);
                }
                row++;
            }
        }
    }


    float elapsedTime = sw.Time()/1000.0; //msec => sec
    SetStatusText(wxString::Format("'%s' imported in %4.3f sec.", filename.GetPath(), elapsedTime));
}

void xLightsFrame::ImportHLS(const wxFileName &filename)
{
    wxStopWatch sw; // start a stopwatch timer

    wxFileName xml_file(filename);
    wxXmlDocument input_xml;
    wxString xml_doc = xml_file.GetFullPath();
    wxFileInputStream fin(xml_doc);

    if( !input_xml.Load(fin) )  return;

    LMSImportChannelMapDialog dlg(this, filename);
    dlg.mSequenceElements = &mSequenceElements;
    dlg.xlights = this;

    /*
     </ChannelData> | </IlluminationData>
     </Channels>
     </Universe>
     </TotalUniverses> | NumberOfTimeCells | MilliSecPerTimeUnit
     </HLS_OutputSequence>
     */
    int frames = 0;
    int frameTime = 0;
    wxXmlNode *totalUniverses = nullptr;
    for (wxXmlNode* tuniv=input_xml.GetRoot()->GetChildren(); tuniv!=NULL; tuniv=tuniv->GetNext()) {
        if (tuniv->GetName() == "NumberOfTimeCells") {
            frames = wxAtoi(tuniv->GetChildren()->GetContent());
        } else if (tuniv->GetName() == "MilliSecPerTimeUnit") {
            frameTime = wxAtoi(tuniv->GetChildren()->GetContent());
        } else if (tuniv->GetName() == "TotalUniverses") {
            totalUniverses = tuniv;
            for (wxXmlNode* univ=tuniv->GetChildren(); univ!=NULL; univ=univ->GetNext()) {
                if (univ->GetName() == "Universe") {
                    for (wxXmlNode* channels=univ->GetChildren(); channels!=NULL; channels=channels->GetNext()) {
                        if (channels->GetName() == "Channels") {
                            for (wxXmlNode* chand=channels->GetChildren(); chand!=NULL; chand=chand->GetNext()) {
                                if (chand->GetName() == "ChannelData") {
                                    for (wxXmlNode* chani=chand->GetChildren(); chani!=NULL; chani=chani->GetNext()) {
                                        if (chani->GetName() == "ChanInfo") {
                                            std::string info = chani->GetChildren()->GetContent().ToStdString();
                                            if (info.find(", Normal") != info.npos) {
                                                std::string name = info.substr(0, info.find(", Normal"));
                                                dlg.channelNames.push_back(name);
                                                dlg.channelColors[name] = xlWHITE;
                                                MapToStrandName(name, dlg.ccrNames);
                                            } else if (info.find(", RGB-") != info.npos) {
                                                std::string name = info.substr(0, info.find(", RGB-"));
                                                std::string color = info.substr(info.size() - 1, 1);
                                                if (color == "R") {
                                                    dlg.channelNames.push_back(name);
                                                    dlg.channelColors[name] = xlBLACK;
                                                }
                                                dlg.channelNames.push_back(info);
                                                if (color == "R") {
                                                    dlg.channelColors[info] = xlRED;
                                                } else if (color == "G") {
                                                    dlg.channelColors[info] = xlGREEN;
                                                } else {
                                                    dlg.channelColors[info] = xlBLUE;
                                                }
                                                MapToStrandName(name, dlg.ccrNames);
                                            } // else "Dead Channel"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::sort(dlg.channelNames.begin(), dlg.channelNames.end());
    dlg.channelNames.insert(dlg.channelNames.begin(), "");
    std::sort(dlg.ccrNames.begin(), dlg.ccrNames.end());
    dlg.ccrNames.insert(dlg.ccrNames.begin(), "");

    dlg.Init();

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    int row = 0;
    for (size_t m = 0; m < dlg.modelNames.size(); m++) {
        std::string modelName = dlg.modelNames[m];
        Model *mc = GetModel(modelName);
        ModelElement * model = nullptr;
        for (size_t i=0;i<mSequenceElements.GetElementCount();i++) {
            if (mSequenceElements.GetElement(i)->GetType() == ELEMENT_TYPE_MODEL
                && modelName == mSequenceElements.GetElement(i)->GetName()) {
                model = dynamic_cast<ModelElement*>(mSequenceElements.GetElement(i));
            }
        }
        MapHLSChannelInformation(this, model->GetEffectLayer(0),
                                 totalUniverses, frames, frameTime,
                                 dlg.ChannelMapGrid->GetCellValue(row, 3),
                                 dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                 *mc, dlg.MapByStrand->GetValue());
        row++;

        if (!dlg.MapByStrand->GetValue()) {
            for (int str = 0; str < mc->GetNumSubModels(); str++) {
                SubModelElement *se =  model->GetSubModel(str);
                EffectLayer *sl = se->GetEffectLayer(0);

                MapHLSChannelInformation(this, sl,
                                         totalUniverses, frames, frameTime,
                                         dlg.ChannelMapGrid->GetCellValue(row, 3),
                                         dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                         *mc, false);
                row++;
            }
        }
        for (int str = 0; str < mc->GetNumStrands(); str++) {
            StrandElement *se =  model->GetStrand(str, true);
            EffectLayer *sl = se->GetEffectLayer(0);

            if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                if (!dlg.MapByStrand->GetValue()) {
                    MapHLSChannelInformation(this, sl,
                                             totalUniverses, frames, frameTime,
                                             dlg.ChannelMapGrid->GetCellValue(row, 3),
                                             dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                             *mc, false);
                } else {
                    std::string ccrName = dlg.ChannelMapGrid->GetCellValue(row, 3).ToStdString();
                    for (int n = 0; n < se->GetNodeLayerCount(); n++) {
                        EffectLayer *layer = se->GetNodeLayer(n, true);

                        wxString nm = FindHLSStrandName(ccrName, n+1, dlg.channelNames);

                        MapHLSChannelInformation(this, layer,
                                                 totalUniverses, frames, frameTime,
                                                 nm,
                                                 dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                                 *mc, true);



                    }
                }
            }
            row++;
            if (!dlg.MapByStrand->GetValue()) {
                for (int n = 0; n < mc->GetStrandLength(str); n++) {
                    if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                        MapHLSChannelInformation(this, se->GetNodeLayer(n, true),
                                                 totalUniverses, frames, frameTime,
                                                 dlg.ChannelMapGrid->GetCellValue(row, 3),
                                                 dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                                 *mc, false);
                    }
                    row++;
                }
            }
        }
    }


    float elapsedTime = sw.Time()/1000.0; //msec => sec
    SetStatusText(wxString::Format("'%s' imported in %4.3f sec.", filename.GetPath(), elapsedTime));
}

void xLightsFrame::ImportLMS(const wxFileName &filename) {
    wxStopWatch sw; // start a stopwatch timer

    wxFileName xml_file(filename);
    wxXmlDocument input_xml;
    wxString xml_doc = xml_file.GetFullPath();
    wxFileInputStream fin(xml_doc);

    if( !input_xml.Load(fin) )  return;
    ImportLMS(input_xml, filename);
    float elapsedTime = sw.Time()/1000.0; //msec => sec
    SetStatusText(wxString::Format("'%s' imported in %4.3f sec.", filename.GetPath(), elapsedTime));
}

void AdjustAllTimings(wxXmlNode *input_xml, int offset) {
    if (input_xml->HasAttribute("startCentisecond")) {
        int i = wxAtoi(input_xml->GetAttribute("startCentisecond"));
        input_xml->DeleteAttribute("startCentisecond");
        input_xml->AddAttribute("startCentisecond", wxString::Format("%d", i + offset));
    }
    if (input_xml->HasAttribute("endCentisecond")) {
        int i = wxAtoi(input_xml->GetAttribute("endCentisecond"));
        input_xml->DeleteAttribute("endCentisecond");
        input_xml->AddAttribute("endCentisecond", wxString::Format("%d", i + offset));
    }
    if (input_xml->GetName() == "flowy") {
        if (input_xml->HasAttribute("startTime")) {
            int i = wxAtoi(input_xml->GetAttribute("startTime"));
            input_xml->DeleteAttribute("startTime");
            input_xml->AddAttribute("startTime", wxString::Format("%d", i + offset));
        }
        if (input_xml->HasAttribute("endTime")) {
            int i = wxAtoi(input_xml->GetAttribute("endTime"));
            input_xml->DeleteAttribute("endTime");
            input_xml->AddAttribute("endTime", wxString::Format("%d", i + offset));
        }
    } else if (input_xml->GetName() == "state1" || input_xml->GetName() == "state2") {
        if (input_xml->HasAttribute("time")) {
            int i = wxAtoi(input_xml->GetAttribute("time"));
            input_xml->DeleteAttribute("time");
            input_xml->AddAttribute("time", wxString::Format("%d", i + offset));
        }
    }
    for (wxXmlNode* chan=input_xml->GetChildren(); chan!=NULL; chan=chan->GetNext()) {
        AdjustAllTimings(chan, offset);
    }
}

void xLightsFrame::ImportSuperStar(const wxFileName &filename)
{
    SuperStarImportDialog dlg(this);

    for(size_t i=0;i<mSequenceElements.GetElementCount();i++) {
        if(mSequenceElements.GetElement(i)->GetType()== ELEMENT_TYPE_MODEL) {
            dlg.ChoiceSuperStarImportModel->Append(mSequenceElements.GetElement(i)->GetName());
        }
    }

    if (dlg.ShowModal() == wxID_CANCEL) {
        return;
    }

    wxString model_name = dlg.ChoiceSuperStarImportModel->GetStringSelection();
    if( model_name == "" )
    {
        wxMessageBox("Please select the target model!");
        return;
    }

    wxStopWatch sw; // start a stopwatch timer
    bool model_found = false;

    // read v3 xml file into temporary document
    wxFileName xml_file(filename);
    wxXmlDocument input_xml;
    wxString xml_doc = xml_file.GetFullPath();
    wxFileInputStream fin(xml_doc);
    FixXMLInputStream bufIn(fin);

    if( !input_xml.Load(bufIn) )  return;


    if (dlg.TimeAdjSpinCtrl->GetValue() != 0) {
        int offset = dlg.TimeAdjSpinCtrl->GetValue();
        AdjustAllTimings(input_xml.GetRoot(), offset  / 10);
    }

    Element* model = nullptr;

    for(size_t i=0;i<mSequenceElements.GetElementCount();i++) {
        if(mSequenceElements.GetElement(i)->GetType()== ELEMENT_TYPE_MODEL) {
            model = mSequenceElements.GetElement(i);
            if( model->GetName() == model_name ) {
                model_found = true;
                break;
            }
        }
    }
    if( model != nullptr && model_found ) {
        int x_size = wxAtoi(dlg.TextCtrl_SS_X_Size->GetValue());
        int y_size = wxAtoi(dlg.TextCtrl_SS_Y_Size->GetValue());
        int x_offset = wxAtoi(dlg.TextCtrl_SS_X_Offset->GetValue());
        int y_offset = wxAtoi(dlg.TextCtrl_SS_Y_Offset->GetValue());
        bool average_colors = dlg.CheckBox_AverageColors->GetValue();
        Model *cls = GetModel(model->GetName());
        int bw, bh;
        cls->GetBufferSize("Default", "None", bw, bh);
        wxSize modelSize(bw, bh);
        ImportSuperStar(model, input_xml, x_size, y_size, x_offset, y_offset, average_colors, dlg.ImageResizeChoice->GetSelection(), modelSize);
    }
    float elapsedTime = sw.Time()/1000.0; //msec => sec
    SetStatusText(wxString::Format("'%s' imported in %4.3f sec.", filename.GetPath(), elapsedTime));
}

bool findRGB(wxXmlNode *e, wxXmlNode *chan, wxXmlNode *&rchannel, wxXmlNode *&gchannel, wxXmlNode *&bchannel) {
    wxString idxs[3];
    int cnt = 0;
    for (wxXmlNode *n = chan->GetChildren(); n != nullptr; n = n->GetNext()) {
        if (n->GetName() == "channels") {
            for (wxXmlNode *n2 = n->GetChildren(); n2 != nullptr; n2 = n2->GetNext()) {
                if (n2->GetName() == "channel" && cnt < 3) {
                    idxs[cnt] = n2->GetAttribute("savedIndex");
                    cnt++;
                }
            }
        }
    }
    for (wxXmlNode* ch=e->GetChildren(); ch!=NULL; ch=ch->GetNext()) {
        if (ch->GetName() == "channel") {
            wxString idx = ch->GetAttribute("savedIndex");
            if (idx == idxs[0]) {
                rchannel = ch;
            }
            if (idx == idxs[1]) {
                gchannel = ch;
            }
            if (idx == idxs[2]) {
                bchannel = ch;
            }
        }
    }
    return true;
}

void GetRGBTimes(wxXmlNode *re, int &startms, int &endms) {
    if (re != nullptr) {
        startms = wxAtoi(re->GetAttribute("startCentisecond")) * 10;
        endms = wxAtoi(re->GetAttribute("endCentisecond")) * 10;
    } else {
        startms = 9999999;
        endms = 9999999;
    }
}
void GetIntensities(wxXmlNode *re, int &starti, int &endi) {
    wxString intensity = re->GetAttribute("intensity", "-1");
    if (intensity == "-1") {
        starti = wxAtoi(re->GetAttribute("startIntensity"));
        endi = wxAtoi(re->GetAttribute("endIntensity"));
    } else {
        starti = endi = wxAtoi(intensity);
    }
}

class RGBData {
public:
    int startms, endms;
    int starti, endi;
    bool shimmer;
};

void FillData(wxXmlNode *nd, RGBData &data) {
    GetIntensities(nd, data.starti, data.endi);
    GetRGBTimes(nd, data.startms, data.endms);
    data.shimmer = nd->GetAttribute("type") == "shimmer";
}
void Insert(int x, std::vector<RGBData> &v, int startms) {
    v.insert(v.begin() + x, 1, RGBData());
    v[x].startms = startms;
    v[x].endms = v[x + 1].startms;
    v[x].endi = v[x].starti = 0;
    v[x].shimmer = false;
}
void Split(int x, std::vector<RGBData> &v, int endms) {
    v.insert(v.begin() + x, 1, RGBData());
    v[x].startms = v[x + 1].startms;
    v[x].endms = endms;
    v[x + 1].startms = endms;
    v[x].shimmer = v[x + 1].shimmer;
    v[x].starti = v[x + 1].starti;
    double d = endms - v[x].startms;
    d = d / double(v[x + 1].endms - v[x].startms);
    double newi = (v[x + 1].endi - v[x].starti) * d + v[x].starti;
    v[x].endi = v[x + 1].starti = newi;
}
#define MAXMS 99999999
int GetStartMS(int x,  std::vector<RGBData> &v) {
    if (x < v.size()) {
        return v[x].startms;
    }
    return MAXMS;
}
int GetEndMS(int x,  std::vector<RGBData> &v) {
    if (x < v.size()) {
        return v[x].endms;
    }
    return MAXMS;
}
void Resize(int x,
            std::vector<RGBData> &v, int startms) {
    while (x >= v.size()) {
        int i = v.size();
        v.push_back(RGBData());
        v[i].endms = MAXMS;
        v[i].startms = startms;
        v[i].starti = v[i].endi = 0;
        v[i].shimmer = false;
    }
}

void UnifyData(int x,
               std::vector<RGBData> &red,
               std::vector<RGBData> &green,
               std::vector<RGBData> &blue) {
    int min = std::min(GetStartMS(x, red), std::min(GetStartMS(x, green), GetStartMS(x ,blue)));
    Resize(x, red, min);
    Resize(x, green, min);
    Resize(x, blue, min);
    if (red[x].startms != min) {
        Insert(x, red, min);
    }
    if (green[x].startms != min) {
        Insert(x, green, min);
    }
    if (blue[x].startms != min) {
        Insert(x, blue, min);
    }
    min = std::min(GetEndMS(x, red), std::min(GetEndMS(x, green), GetEndMS(x ,blue)));
    if (min == MAXMS) {
        return;
    }
    if (red[x].endms != min) {
        Split(x, red, min);
    }
    if (green[x].endms != min) {
        Split(x, green, min);
    }
    if (blue[x].endms != min) {
        Split(x, blue, min);
    }

}
bool GetRGBEffectData(RGBData &red, RGBData &green, RGBData &blue, xlColor &sc, xlColor &ec) {

    sc.red = red.starti * 255 / 100;
    sc.green = green.starti * 255 / 100;
    sc.blue = blue.starti * 255 / 100;

    ec.red = red.endi * 255 / 100;
    ec.green = green.endi * 255 / 100;
    ec.blue = blue.endi * 255 / 100;

    return red.shimmer | blue.shimmer | green.shimmer;
}

void LoadRGBData(EffectManager &effectManager, EffectLayer *layer, wxXmlNode *rchannel, wxXmlNode *gchannel, wxXmlNode *bchannel) {
    std::vector<RGBData> red, green, blue;
    while (rchannel != nullptr) {
        red.resize(red.size() + 1);
        FillData(rchannel, red[red.size() - 1]);
        rchannel = rchannel->GetNext();
    }
    while (gchannel != nullptr) {
        green.resize(green.size() + 1);
        FillData(gchannel, green[green.size() - 1]);
        gchannel = gchannel->GetNext();
    }
    while (bchannel != nullptr) {
        blue.resize(blue.size() + 1);
        FillData(bchannel, blue[blue.size() - 1]);
        bchannel = bchannel->GetNext();
    }
    //have the data, now need to split it so common start/end times
    for (size_t x = 0; x < red.size() || x < green.size() || x < blue.size(); x++) {
        UnifyData(x, red, green, blue);
    }

    for (size_t x = 0; x < red.size() || x < green.size() || x < blue.size(); x++) {
        xlColor sc, ec;
        bool isShimmer = GetRGBEffectData(red[x], green[x], blue[x], sc, ec);

        int starttime = red[x].startms;
        int endtime = red[x].endms;

        if (ec == sc) {
            if (ec != xlBLACK) {
                std::string palette = "C_BUTTON_Palette1=" + (std::string)sc + ",C_CHECKBOX_Palette1=1,"
                    + "C_BUTTON_Palette2=#000000,C_CHECKBOX_Palette2=0";
                std::string settings = (isShimmer ? "E_CHECKBOX_On_Shimmer=1" : "");
                layer->AddEffect(0, "On", settings, palette, starttime, endtime, false, false);
            }
        } else if (sc == xlBLACK) {
            std::string palette = "C_BUTTON_Palette1=" + (std::string)ec + ",C_CHECKBOX_Palette1=1,"
                "C_BUTTON_Palette2=#000000,C_CHECKBOX_Palette2=0";
            std::string settings = "E_TEXTCTRL_Eff_On_Start=0";
            if (isShimmer) {
                settings += ",E_CHECKBOX_On_Shimmer=1";
            }
            layer->AddEffect(0, "On", settings, palette, starttime, endtime, false, false);
        } else if (ec == xlBLACK) {
            std::string palette = "C_BUTTON_Palette1=" + (std::string)sc + ",C_CHECKBOX_Palette1=1,"
                "C_BUTTON_Palette2=#000000,C_CHECKBOX_Palette2=0";
            std::string settings = "E_TEXTCTRL_Eff_On_End=0";
            if (isShimmer) {
                settings += ",E_CHECKBOX_On_Shimmer=1";
            }
            layer->AddEffect(0, "On", settings, palette, starttime, endtime, false, false);
        } else {
            std::string palette = "C_BUTTON_Palette1=" + (std::string)sc + ",C_CHECKBOX_Palette1=1,"
                "C_BUTTON_Palette2=" + (std::string)ec + ",C_CHECKBOX_Palette2=1";
            std::string settings = (isShimmer ? "E_CHECKBOX_ColorWash_Shimmer=1," : "");
            layer->AddEffect(0, "Color Wash", settings, palette, starttime, endtime, false, false);
        }
    }
}

void MapRGBEffects(EffectManager &effectManager, EffectLayer *layer, wxXmlNode *rchannel, wxXmlNode *gchannel, wxXmlNode *bchannel) {
    wxXmlNode* re=rchannel->GetChildren();
    while (re != nullptr && "effect" != re->GetName()) re = re->GetNext();
    wxXmlNode* ge=gchannel->GetChildren();
    while (ge != nullptr && "effect" != ge->GetName()) ge = ge->GetNext();
    wxXmlNode* be=bchannel->GetChildren();
    while (be != nullptr && "effect" != be->GetName()) be = be->GetNext();
    LoadRGBData(effectManager, layer, re, ge, be);
}
void MapOnEffects(EffectManager &effectManager, EffectLayer *layer, wxXmlNode *channel, int chancountpernode, const wxColor &color) {
    std::string palette = "C_BUTTON_Palette1=#FFFFFF,C_CHECKBOX_Palette1=1";
    if (chancountpernode > 1) {
        wxString c = wxString::Format("#%06lx",color.GetRGB());
        xlColor color1(c);
        palette = "C_BUTTON_Palette1=" + color1 + ",C_CHECKBOX_Palette1=1";
    }

    for (wxXmlNode* ch=channel->GetChildren(); ch!=NULL; ch=ch->GetNext()) {
        if (ch->GetName() == "effect") {
            int starttime = (wxAtoi(ch->GetAttribute("startCentisecond"))) * 10;
            int endtime = (wxAtoi(ch->GetAttribute("endCentisecond"))) * 10;
            std::string intensity = ch->GetAttribute("intensity", "-1").ToStdString();
            std::string starti, endi;
            if (intensity == "-1") {
                starti = ch->GetAttribute("startIntensity").ToStdString();
                endi = ch->GetAttribute("endIntensity").ToStdString();
            } else {
                starti = endi = intensity;
            }
            std::string settings;
            if (100 != starti) {
                settings += "E_TEXTCTRL_Eff_On_Start=" + starti;
            }
            if (100 != endi) {
                if (!settings.empty()) {
                    settings += ",";
                }
                settings += "E_TEXTCTRL_Eff_On_End=" + endi;
            }
            if ("intensity" != ch->GetAttribute("type")) {
                if (!settings.empty()) {
                    settings += ",";
                }
                settings += "E_CHECKBOX_On_Shimmer=1";
            }
            layer->AddEffect(0, "On", settings, palette, starttime, endtime, false, false);
        }
    }
}
bool MapChannelInformation(EffectManager &effectManager, EffectLayer *layer, wxXmlDocument &input_xml, const wxString &nm, const wxColor &color, const Model &mc) {
    if ("" == nm) {
        return false;
    }
    wxXmlNode *channel = nullptr;
    wxXmlNode *rchannel = nullptr;
    wxXmlNode *gchannel = nullptr;
    wxXmlNode *bchannel = nullptr;
    for(wxXmlNode* e=input_xml.GetRoot()->GetChildren(); e!=NULL; e=e->GetNext()) {
        if (e->GetName() == "channels"){
            for (wxXmlNode* chan=e->GetChildren(); chan!=NULL; chan=chan->GetNext()) {
                if ((chan->GetName() == "channel" || chan->GetName() == "rgbChannel")
                    && nm == chan->GetAttribute("name")) {
                    channel = chan;
                    if (chan->GetName() == "rgbChannel"
                        && !findRGB(e, chan, rchannel, gchannel, bchannel)) {
                        return false;
                    }
                    break;
                }
            }
        }
    }
    if (channel == nullptr) {
        return false;
    }
    if (channel->GetName() == "rgbChannel") {
        MapRGBEffects(effectManager, layer, rchannel, gchannel, bchannel);
    } else {
        MapOnEffects(effectManager, layer, channel, mc.GetChanCountPerNode(), color);
    }
    return true;
}
bool xLightsFrame::ImportLMS(wxXmlDocument &input_xml, const wxFileName &filename)
{
    LMSImportChannelMapDialog dlg(this, filename);
    dlg.mSequenceElements = &mSequenceElements;
    dlg.TimeAdjustPanel->Show();
    dlg.xlights = this;

    for(wxXmlNode* e=input_xml.GetRoot()->GetChildren(); e!=NULL; e=e->GetNext()) {
        if (e->GetName() == "channels"){
            for (wxXmlNode* chan=e->GetChildren(); chan!=NULL; chan=chan->GetNext()) {
                if (chan->GetName() == "channel" || chan->GetName() == "rgbChannel") {
                    std::string name = chan->GetAttribute("name").ToStdString();
                    if (chan->GetName() == "rgbChannel") {
                        dlg.channelColors[name] = xlBLACK;
                    } else {
                        std::string color = chan->GetAttribute("color").ToStdString();
                        dlg.channelColors[name] = GetColor(color);
                    }

                    dlg.channelNames.push_back(name);
                    if (chan->GetName() == "rgbChannel") {
                        int idxDP = name.find("-P");
                        int idxUP = name.find(" P");
                        int idxSP = name.find(" p");
                        if (idxUP > idxSP) {
                            idxSP = idxUP;
                        }
                        if (idxDP > idxSP) {
                            idxSP = idxDP;
                        }
                        if (idxSP != wxNOT_FOUND) {
                            int i = wxAtoi(name.substr(idxSP + 2, name.size()));
                            if (i > 0
                                && (dlg.ccrNames.size() == 0 || dlg.ccrNames.back() != name.substr(0, idxSP - 1))) {
                                dlg.ccrNames.push_back(name.substr(0, idxSP - 1));
                            }
                        }
                    }
                }
            }
        }
    }
    std::sort(dlg.channelNames.begin(), dlg.channelNames.end());
    dlg.channelNames.insert(dlg.channelNames.begin(), "");
    std::sort(dlg.ccrNames.begin(), dlg.ccrNames.end());
    dlg.ccrNames.insert(dlg.ccrNames.begin(), "");

    dlg.Init();

    if (dlg.ShowModal() != wxID_OK) {
        return false;
    }

    if (dlg.TimeAdjustSpinCtrl->GetValue() != 0) {
        int offset = dlg.TimeAdjustSpinCtrl->GetValue();
        AdjustAllTimings(input_xml.GetRoot(), offset / 10);
    }

    int row = 0;
    for (size_t m = 0; m < dlg.modelNames.size(); m++) {
        std::string modelName = dlg.modelNames[m];
        Model *mc = GetModel(modelName);
        ModelElement * model = nullptr;
        for (size_t i=0;i<mSequenceElements.GetElementCount();i++) {
            if (mSequenceElements.GetElement(i)->GetType() == ELEMENT_TYPE_MODEL
                && modelName == mSequenceElements.GetElement(i)->GetName()) {
                model = dynamic_cast<ModelElement*>(mSequenceElements.GetElement(i));
            }
        }
        MapChannelInformation(effectManager,
                              model->GetEffectLayer(0), input_xml,
                              dlg.ChannelMapGrid->GetCellValue(row, 3),
                              dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4), *mc);
        row++;
        if (!dlg.MapByStrand->GetValue()) {
            for (int str = 0; str < mc->GetNumSubModels(); str++) {
                SubModelElement *sm = model->GetSubModel(str);
                EffectLayer *sl = sm->GetEffectLayer(0);
                MapChannelInformation(effectManager, sl,
                                      input_xml,
                                      dlg.ChannelMapGrid->GetCellValue(row, 3),
                                      dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4), *mc);
                row++;
            }
        }
        for (int str = 0; str < mc->GetNumStrands(); str++) {
            StrandElement *se = model->GetStrand(str, true);
            EffectLayer *sl = se->GetEffectLayer(0);

            if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                if (!dlg.MapByStrand->GetValue()) {
                    MapChannelInformation(effectManager, sl,
                                      input_xml,
                                      dlg.ChannelMapGrid->GetCellValue(row, 3),
                                      dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4), *mc);
                } else {
                    wxString ccrName = dlg.ChannelMapGrid->GetCellValue(row, 3);
                    for (int n = 0; n < se->GetNodeLayerCount(); n++) {
                        EffectLayer *layer = se->GetNodeLayer(n, true);
                        wxString nm = ccrName + wxString::Format("-P%02d", (n + 1));
                        if (std::find(dlg.channelNames.begin(), dlg.channelNames.end(), nm) == dlg.channelNames.end()) {
                            nm = ccrName + wxString::Format(" p%02d", (n + 1));
                        }
                        if (std::find(dlg.channelNames.begin(), dlg.channelNames.end(), nm) == dlg.channelNames.end()) {
                            nm = ccrName + wxString::Format("-P%d", (n + 1));
                        }
                        if (std::find(dlg.channelNames.begin(), dlg.channelNames.end(), nm) == dlg.channelNames.end()) {
                            nm = ccrName + wxString::Format(" p%d", (n + 1));
                        }
                        if (std::find(dlg.channelNames.begin(), dlg.channelNames.end(), nm) == dlg.channelNames.end()) {
                            nm = ccrName + wxString::Format(" P %02d", (n + 1));
                        }
                        MapChannelInformation(effectManager, layer,
                                              input_xml,
                                              nm,
                                              dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                              *mc);
                    }
                }
            }
            row++;
            if (!dlg.MapByStrand->GetValue()) {
                for (int n = 0; n < mc->GetStrandLength(str); n++) {
                    if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                        MapChannelInformation(effectManager, se->GetNodeLayer(n, true),
                                              input_xml,
                                              dlg.ChannelMapGrid->GetCellValue(row, 3),
                                              dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4),
                                              *mc);
                    }
                    row++;
                }
            }
        }

    }

    return true;
}
unsigned char ChannelBlend(unsigned char c1, unsigned char  c2, double ratio)
{
    return c1 + floor(ratio*(c2-c1)+0.5);
}

class ImageInfo {
public:
    int xOffset;
    int yOffset;
    int width;
    int height;
    double scaleY;
    double scaleX;
    std::string imageName;

    void Set(int x, int y, int w, int h, const std::string &n) {
        xOffset = x;
        yOffset = y;
        width = w;
        height = h;
        imageName = n;
        scaleX = 1.0;
        scaleY = 1.0;
    }
};

void ScaleImage(wxImage &img, int type,
                const wxSize &modelSize,
                int numCol, int numRow,
                ImageInfo &imgInfo,
                bool reverse) {
    bool scale = false;

    imgInfo.xOffset = imgInfo.xOffset + (imgInfo.width-numCol)/2;
    if( reverse ) {
        imgInfo.yOffset = imgInfo.yOffset + (imgInfo.height+0.5)/2 - numRow/2;
    } else {
        imgInfo.yOffset = numRow - imgInfo.yOffset - (numRow+imgInfo.height)/2;
    }

    switch (type) {
        case 0: // NONE
            return;
        case 1: // Exact width
            if (numCol == imgInfo.width) {
                imgInfo.width = modelSize.GetWidth();
                imgInfo.scaleX = ((double)modelSize.GetWidth())/((double)numCol);
                imgInfo.xOffset = round((double)imgInfo.xOffset * imgInfo.scaleX);
                scale = true;
            }
            break;
        case 2: // Exact height
            if (numRow == imgInfo.height) {
                imgInfo.height = modelSize.GetHeight();
                imgInfo.scaleY = ((double)modelSize.GetHeight())/((double)numRow);
                imgInfo.yOffset = round((double)imgInfo.yOffset * imgInfo.scaleY);
                scale = true;
            }
            break;
        case 3: // Exact width or height
            if (numCol == imgInfo.width) {
                imgInfo.width = modelSize.GetWidth();
                imgInfo.scaleX = ((double)modelSize.GetWidth())/((double)numCol);
                imgInfo.xOffset = round((double)imgInfo.xOffset * imgInfo.scaleX);
                scale = true;
            }
            if (numRow == imgInfo.height) {
                imgInfo.height = modelSize.GetHeight();
                imgInfo.scaleY = ((double)modelSize.GetHeight())/((double)numRow);
                imgInfo.yOffset = round((double)imgInfo.yOffset * imgInfo.scaleY);
                scale = true;
            }
            break;
        case 4: // everything
            imgInfo.scaleX = ((double)modelSize.GetWidth())/((double)numCol);
            imgInfo.scaleY = ((double)modelSize.GetHeight())/((double)numRow);
            int newW = round((double)imgInfo.width * imgInfo.scaleX);
            int newH = round((double)imgInfo.height * imgInfo.scaleY);
            if (newH != imgInfo.height || newW !=imgInfo.width) {
                scale = true;
                imgInfo.height = newH;
                imgInfo.width = newW;
                imgInfo.yOffset = round((double)imgInfo.yOffset * imgInfo.scaleY);
                imgInfo.xOffset = round((double)imgInfo.xOffset * imgInfo.scaleX);
            }
            break;
    }
    if (scale) {
        img.Rescale(imgInfo.width, imgInfo.height);
    }
}

wxString CreateSceneImage(const std::string &imagePfx, const std::string &postFix,
                          wxXmlNode *element, int numCols,
                          int numRows, bool reverse, bool rotate, const xlColor &color, int y_offset,
                          int resizeType, const wxSize &modelSize) {
    wxImage i;
    i.Create(numCols, numRows);
    i.InitAlpha();
    for (int x = 0; x < numCols; x++)  {
        for (int y = 0; y < numRows; y++) {
            i.SetAlpha(x, y, wxALPHA_TRANSPARENT);
        }
    }
    for(wxXmlNode* e=element->GetChildren(); e!=NULL; e=e->GetNext()) {
        if (e->GetName() == "element") {
            int x = wxAtoi(e->GetAttribute("ribbonIndex"));
            int y = wxAtoi(e->GetAttribute("pixelIndex")) - y_offset;
            if(rotate) {
                std::swap(x, y);
            }
            if( rotate ^ reverse ) {
                y = numRows - y;
            }
            if (x < numCols && y >=0 && y < numRows) {
                i.SetRGB(x, y, color.Red(), color.Green(), color.Blue());
                i.SetAlpha(x, y, wxALPHA_OPAQUE);
            }
        }
    }
    std::string name = imagePfx + "_s" + element->GetAttribute("savedIndex").ToStdString() + postFix + ".png";
    ImageInfo im;
    im.Set(0, 0, numCols, numRows, name);
    ScaleImage(i, resizeType, modelSize, numCols, numRows, im, false);
    i.SaveFile(name);
    return name;
}
bool IsPartOfModel(wxXmlNode *element, int num_rows, int num_columns, bool &isFull, wxRect &rect, bool reverse) {
    std::vector< std::vector<bool> > data(num_columns, std::vector<bool>(num_rows));
    int maxCol = -1;
    int maxRow = -1;
    int minCol = 9999999;
    int minRow = 9999999;
    isFull = true;
    for(wxXmlNode* e=element->GetChildren(); e!=NULL; e=e->GetNext()) {
        if (e->GetName() == "element") {
            int x = wxAtoi(e->GetAttribute("ribbonIndex"));
            int y = wxAtoi(e->GetAttribute("pixelIndex"));
            if( reverse ) {
                std::swap( x, y );
            }
            if (x < num_columns) {
                data[x][y] = true;
                if (x > maxCol) maxCol = x;
                if (x < minCol) minCol = x;
                if (y > maxRow) maxRow = y;
                if (y < minRow) minRow = y;
            } else {
                return false;
            }
        }
    }
    isFull = minCol == 0 && minRow == 0 && maxRow == (num_rows - 1) && maxCol == (num_columns - 1);
    bool isRect = true;
    for (int x = minCol; x <= maxCol; x++) {
        for (int y = minRow; y <= maxRow; y++) {
            if (!data[x][y]) {
                isFull = false;
                isRect = false;
            }
        }
    }
    if (isRect) {
        rect.x = minCol;
        rect.y = minRow;
        rect.width = maxCol;
        rect.height = maxRow;
    } else {
        rect.x = -1;
        rect.y = -1;
    }
    return true;
}

bool xLightsFrame::ImportSuperStar(Element *model, wxXmlDocument &input_xml, int x_size, int y_size,
                                   int x_offset, int y_offset, bool average_colors,
                                   int imageResizeType, const wxSize &modelSize)
{
    double num_rows = 1.0;
    double num_columns = 1.0;
    bool reverse_rows = false;
    bool reverse_xy = false;
    bool layout_defined = false;
    wxXmlNode* input_root=input_xml.GetRoot();
    EffectLayer* layer = model->AddEffectLayer();
    std::map<int, ImageInfo> imageInfo;
    std::string imagePfx;
    std::vector<bool> reserved;
    std::string blend_string = "";
    if( average_colors ) {
        blend_string = ",T_CHOICE_LayerMethod=Average";
    }
    for(wxXmlNode* e=input_root->GetChildren(); e!=NULL; e=e->GetNext()) {
        if ("imageActions" == e->GetName()) {
            for(wxXmlNode* element=e->GetChildren(); element!=NULL; element=element->GetNext()) {
                if ("imageAction" == element->GetName()) {
                    int layer_index = wxAtoi(element->GetAttribute("layer"));
                    if (layer_index > 0) layer_index--;
                    if (layer_index >= reserved.size()) {
                        reserved.resize(layer_index + 1, false);
                    }
                    reserved[layer_index] = true;
                }
            }
        } else if ("scenes" == e->GetName() || "images" == e->GetName()) {
            for(wxXmlNode* element=e->GetChildren(); (element != NULL) && ("" == imagePfx); element=element->GetNext()) {
                if ("image" == element->GetName() || "scene" == element->GetName()) {
                    if ("" == imagePfx) {
                        wxFileDialog fd(this,
                                        "Choose location and base name for image files",
                                        showDirectory,
                                        wxEmptyString,
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFD_SAVE);
                        if (fd.ShowModal() == wxID_CANCEL) {
                            return false;
                        }
                        imagePfx = fd.GetPath().ToStdString();
                    }
                }
            }
        } else if (e->GetName() == "layouts") {
            wxXmlNode* element=e->GetChildren();
            wxString attr;
            element->GetAttribute("visualizationMode", &attr);
            if( attr == "false" )
            {
                element->GetAttribute("nbrOfRibbons", &attr);
                attr.ToDouble(&num_columns);
                num_rows = 50.0;
                element->GetAttribute("ribbonLength", &attr);
                if( attr == "half" )
                {
                    num_rows /= 2.0;
                    num_columns *= 2.0;
                }
            }
            else
            {
                num_rows = (double)y_size;
                num_columns = (double)x_size;
            }
            element->GetAttribute("ribbonOrientation", &attr);
            if( attr == "horizontal" )
            {
                reverse_xy = true;
                std::swap(num_columns, num_rows);
            }
            else
            {
                reverse_rows = true;
            }
            layout_defined = true;
        }
    }
    for(wxXmlNode* e=input_root->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "morphs")
        {
            if( !layout_defined )
            {
                wxMessageBox("The layouts section was not found in the SuperStar file!");
                return false;
            }
            for(wxXmlNode* element=e->GetChildren(); element!=NULL; element=element->GetNext() )
            {
                int layer_index;
                double layer_val;
                wxString name_attr;
                wxString acceleration;
                wxString state1_time, state2_time, ramp_time_ext;
                std::string attr;
                int start_time, end_time, ramp_time;
                element->GetAttribute("name", &name_attr);
                element->GetAttribute("acceleration", &acceleration);
                attr = element->GetAttribute("layer");
                layer_val = atof(attr.c_str());
                layer_index = (int)layer_val;
                wxXmlNode* state1=element->GetChildren();
                wxXmlNode* state2=state1->GetNext();
                wxXmlNode* ramp=state2->GetNext();
                state1->GetAttribute("time", &state1_time);
                state2->GetAttribute("time", &state2_time);
                ramp->GetAttribute("timeExt", &ramp_time_ext);
                start_time = wxAtoi(state1_time) * 10;
                end_time = wxAtoi(state2_time) * 10;
                ramp_time = wxAtoi(ramp_time_ext) * 10;
                end_time += ramp_time;
                double head_duration = (1.0 - (double)ramp_time/((double)end_time-(double)start_time)) * 100.0;
                std::string settings = "E_CHECKBOX_Morph_End_Link=0,E_CHECKBOX_Morph_Start_Link=0,E_CHECKBOX_ShowHeadAtStart=0,E_NOTEBOOK_Morph=Start,E_SLIDER_MorphAccel=0,E_SLIDER_Morph_Repeat_Count=0,E_SLIDER_Morph_Repeat_Skip=1,E_SLIDER_Morph_Stagger=0";
                settings += acceleration + ",";
                wxString duration = wxString::Format("E_SLIDER_MorphDuration=%d,",(int)head_duration);
                settings += duration;
                attr = state2->GetAttribute("trailLen");
                settings += "E_SLIDER_MorphEndLength=" + attr + ",";
                attr = state1->GetAttribute("trailLen");
                settings += "E_SLIDER_MorphStartLength=" + attr + ",";
                if( !reverse_xy ) attr = state2->GetAttribute("x1");
                else              attr = state2->GetAttribute("y1");
                if( !CalcPercentage(attr, num_columns, false, x_offset) ) continue;
                settings += "E_SLIDER_Morph_End_X1=" + attr + ",";
                if( !reverse_xy ) attr = state2->GetAttribute("x2");
                else              attr = state2->GetAttribute("y2");
                if( !CalcPercentage(attr, num_columns, false, x_offset) ) continue;
                settings += "E_SLIDER_Morph_End_X2=" + attr + ",";
                if( !reverse_xy ) attr = state2->GetAttribute("y1");
                else              attr = state2->GetAttribute("x1");
                if( !CalcPercentage(attr, num_rows, reverse_rows, y_offset) ) continue;
                settings += "E_SLIDER_Morph_End_Y1=" + attr + ",";
                if( !reverse_xy ) attr = state2->GetAttribute("y2");
                else              attr = state2->GetAttribute("x2");
                if( !CalcPercentage(attr, num_rows, reverse_rows, y_offset) ) continue;
                settings += "E_SLIDER_Morph_End_Y2=" + attr + ",";
                if( !reverse_xy ) attr = state1->GetAttribute("x1");
                else              attr = state1->GetAttribute("y1");
                if( !CalcPercentage(attr, num_columns, false, x_offset) ) continue;
                settings += "E_SLIDER_Morph_Start_X1=" + attr + ",";
                if( !reverse_xy ) attr = state1->GetAttribute("x2");
                else              attr = state1->GetAttribute("y2");
                if( !CalcPercentage(attr, num_columns, false, x_offset) ) continue;
                settings += "E_SLIDER_Morph_Start_X2=" + attr + ",";
                if( !reverse_xy ) attr = state1->GetAttribute("y1");
                else              attr = state1->GetAttribute("x1");
                if( !CalcPercentage(attr, num_rows, reverse_rows, y_offset) ) continue;
                settings += "E_SLIDER_Morph_Start_Y1=" + attr + ",";
                if( !reverse_xy ) attr = state1->GetAttribute("y2");
                else              attr = state1->GetAttribute("x2");
                if( !CalcPercentage(attr, num_rows, reverse_rows, y_offset) ) continue;
                settings += "E_SLIDER_Morph_Start_Y2=" + attr + ",";
                std::string sRed, sGreen, sBlue,color;
                sRed = state1->GetAttribute("red");
                sGreen = state1->GetAttribute("green");
                sBlue = state1->GetAttribute("blue");
                color = GetColorString(sRed, sGreen, sBlue);
                std::string palette = "C_BUTTON_Palette1=" + (std::string)color + ",";
                sRed = state2->GetAttribute("red");
                sGreen = state2->GetAttribute("green");
                sBlue = state2->GetAttribute("blue");
                color = GetColorString(sRed, sGreen, sBlue);
                palette += "C_BUTTON_Palette2=" + color + ",";
                sRed = ramp->GetAttribute("red1");
                sGreen = ramp->GetAttribute("green1");
                sBlue = ramp->GetAttribute("blue1");
                color = GetColorString(sRed, sGreen, sBlue);
                palette += "C_BUTTON_Palette3=" + color + ",";
                sRed = ramp->GetAttribute("red2");
                sGreen = ramp->GetAttribute("green2");
                sBlue = ramp->GetAttribute("blue2");
                color = GetColorString(sRed, sGreen, sBlue);
                palette += "C_BUTTON_Palette4=" + color + ",";
                palette += "C_BUTTON_Palette5=#FFFFFF,C_BUTTON_Palette6=#000000,C_CHECKBOX_Palette1=1,C_CHECKBOX_Palette2=1,C_CHECKBOX_Palette3=1,C_CHECKBOX_Palette4=1,";
                if( color != xlBLACK ) {
                    if( average_colors ) {
                        settings += blend_string;
                        settings += ",";
                    } else {
                        settings += ",T_CHOICE_LayerMethod=1 reveals 2,";
                    }
                } else {
                    if( average_colors ) {
                        settings += blend_string;
                        settings += ",";
                    }
                }
                while( model->GetEffectLayerCount() < layer_index )
                {
                    model->AddEffectLayer();
                }
                layer = FindOpenLayer(model, layer_index, start_time, end_time, reserved);
                layer->AddEffect(0, "Morph", settings, palette, start_time, end_time, false, false);
            }
        } else if ("images" == e->GetName()) {
            for(wxXmlNode* element=e->GetChildren(); element!=NULL; element=element->GetNext()) {
                if ("image" == element->GetName()) {
                    for(wxXmlNode* i=element->GetChildren(); i!=NULL; i=i->GetNext()) {
                        if ("pixe" == i->GetName()){
                            wxString data = i->GetAttribute("s");
                            int w = wxAtoi(element->GetAttribute("width"));
                            int h = wxAtoi(element->GetAttribute("height"));


                            int idx = wxAtoi(element->GetAttribute("savedIndex"));
                            int xOffset =  wxAtoi(element->GetAttribute("xOffset"));
                            int yOffset =  wxAtoi(element->GetAttribute("yOffset"));

                            unsigned char *bytes = (unsigned char *)malloc(w*h*3);
                            unsigned char *alpha = (unsigned char *)malloc(w*h);
                            int cnt = 0;
                            int p = 0;
                            wxStringTokenizer tokenizer(data, ",");
                            while (tokenizer.HasMoreTokens()) {
                                unsigned int ii = wxAtoi(tokenizer.GetNextToken());
                                unsigned int v = (ii >> 16) & 0xff;
                                v *= 255;
                                v /= 100;
                                bytes[cnt] = v;
                                v = (ii >> 8) & 0xff;
                                v *= 255;
                                v /= 100;
                                bytes[cnt + 1] = v;
                                v = ii & 0xff;
                                v *= 255;
                                v /= 100;
                                bytes[cnt + 2] = v;

                                alpha[p] = wxALPHA_OPAQUE;
                                if (ii == 0) {
                                    alpha[p] = wxALPHA_TRANSPARENT;
                                }
                                p++;
                                cnt += 3;
                            }

                            wxImage image(w, h, bytes, alpha);
                            if ("" == imagePfx) {
                                wxFileDialog fd(this,
                                                "Choose location and base name for image files",
                                                showDirectory,
                                                wxEmptyString,
                                                wxFileSelectorDefaultWildcardStr,
                                                wxFD_SAVE);
                                while (fd.ShowModal() == wxID_CANCEL || fd.GetFilename() == "") {
                                }
                                imagePfx = fd.GetPath();
                            }
                            std::string fname = imagePfx + "_" + wxString::Format("%d.png", idx).ToStdString();
                            if( reverse_xy ) {
                                image = image.Rotate90(false);
                                imageInfo[idx].Set(yOffset, xOffset, h, w, fname);
                            } else {
                                imageInfo[idx].Set(xOffset, yOffset, w, h, fname);
                            }
                            ScaleImage(image, imageResizeType, modelSize, num_columns, num_rows, imageInfo[idx], reverse_xy);
                            image.SaveFile(fname);
                        }
                    }
                }
            }
        } else if ("flowys" == e->GetName()) {
            for(wxXmlNode* element=e->GetChildren(); element!=NULL; element=element->GetNext()) {
                if ("flowy" == element->GetName()) {
                    std::string centerX, centerY;
                    int startms = wxAtoi(element->GetAttribute("startTime")) * 10;
                    int endms = wxAtoi(element->GetAttribute("endTime")) * 10;
                    wxString type = element->GetAttribute("flowyType");
                    wxString color_string = element->GetAttribute("Colors");
                    std::string sRed, sGreen, sBlue, color;
                    std::string palette = "C_BUTTON_Palette1=" + (std::string)color + ",";
                    int cnt = 1;
                    wxStringTokenizer tokenizer(color_string, " ");
                    while (tokenizer.HasMoreTokens() && cnt <=6) {
                        wxStringTokenizer tokenizer2(tokenizer.GetNextToken(), ",");
                        sRed = tokenizer2.GetNextToken();
                        sGreen = tokenizer2.GetNextToken();
                        sBlue = tokenizer2.GetNextToken();
                        color = GetColorString(sRed, sGreen, sBlue);
                        if( cnt > 1 ) {
                            palette += ",";
                        }
                        palette += "C_BUTTON_Palette" + wxString::Format("%d", cnt) + "=" + color;
                        palette += ",C_CHECKBOX_Palette" + wxString::Format("%d", cnt) + "=1";
                        cnt++;
                    }
                    while (cnt<=6) {
                        if( cnt > 1 ) {
                            palette += ",";
                        }
                        palette += "C_BUTTON_Palette" + wxString::Format("%d", cnt) + "=#000000";
                        palette += ",C_CHECKBOX_Palette" + wxString::Format("%d", cnt) + "=0";
                        cnt++;
                    }

                    int layer_index = wxAtoi(element->GetAttribute("layer"));
                    int acceleration = wxAtoi(element->GetAttribute("acceleration"));
                    if( !reverse_xy ) centerX = element->GetAttribute("centerX").ToStdString();
                    else              centerX = element->GetAttribute("centerY").ToStdString();
                    if( !CalcPercentage(centerX, num_columns, false, x_offset) ) continue;
                    if( !reverse_xy ) centerY = element->GetAttribute("centerY").ToStdString();
                    else              centerY = element->GetAttribute("centerX").ToStdString();
                    if( !CalcPercentage(centerY, num_rows, reverse_rows, y_offset) ) continue;
                    int startAngle = wxAtoi(element->GetAttribute("startAngle"));
                    int endAngle = wxAtoi(element->GetAttribute("endAngle"));
                    if( reverse_xy ) {
                        startAngle -= 90;
                        endAngle -= 90;
                        if( startAngle < 0 ) {
                            startAngle += 360;
                        }
                        if( endAngle < 0 ) {
                            endAngle += 360;
                        }
                    }
                    int revolutions = std::abs(endAngle-startAngle);
                    if( revolutions == 0 ) revolutions = 3;  // algorithm needs non-zero value until we figure out better way to draw effect
                    int startRadius = wxAtoi(element->GetAttribute("startRadius"));
                    int endRadius = wxAtoi(element->GetAttribute("endRadius"));
                    layer = FindOpenLayer(model, layer_index, startms, endms, reserved);
                    if( type == "Spiral" )
                    {
                        int tailms = wxAtoi(element->GetAttribute("tailTimeLength")) * 10;
                        endms += tailms;
                        double duration = (1.0 - (double)tailms/((double)endms-(double)startms)) * 100.0;
                        int startWidth = wxAtoi(element->GetAttribute("startDotSize"));
                        int endWidth = wxAtoi(element->GetAttribute("endDotSize"));
                        std::string settings = "E_CHECKBOX_Galaxy_Reverse=" + wxString::Format("%d", endAngle < startAngle).ToStdString()
                                            + ",E_CHECKBOX_Galaxy_Blend_Edges=1"
                                            + ",E_CHECKBOX_Galaxy_Inward=1"
                                            + ",E_NOTEBOOK_Galaxy=Start,E_SLIDER_Galaxy_Accel=" + wxString::Format("%d", acceleration).ToStdString()
                                            + ",E_SLIDER_Galaxy_CenterX=" + centerX
                                            + ",E_SLIDER_Galaxy_CenterY=" + centerY
                                            + ",E_SLIDER_Galaxy_Duration=" + wxString::Format("%d", (int)duration).ToStdString()
                                            + ",E_SLIDER_Galaxy_End_Radius=" + wxString::Format("%d", endRadius).ToStdString()
                                            + ",E_SLIDER_Galaxy_End_Width=" + wxString::Format("%d", endWidth).ToStdString()
                                            + ",E_SLIDER_Galaxy_Revolutions=" + wxString::Format("%d", revolutions).ToStdString()
                                            + ",E_SLIDER_Galaxy_Start_Angle=" + wxString::Format("%d", startAngle).ToStdString()
                                            + ",E_SLIDER_Galaxy_Start_Radius=" + wxString::Format("%d", startRadius).ToStdString()
                                            + ",E_SLIDER_Galaxy_Start_Width=" + wxString::Format("%d", startWidth).ToStdString()
                                            + blend_string;

                        layer->AddEffect(0, "Galaxy", settings, palette, startms, endms, false, false);
                    }
                    else if( type == "Shockwave" )
                    {
                        int startWidth = wxAtoi(element->GetAttribute("headWidth"));
                        int endWidth = wxAtoi(element->GetAttribute("tailWidth"));
                        std::string settings = "E_CHECKBOX_Shockwave_Blend_Edges=1,E_NOTEBOOK_Shockwave=Position,E_SLIDER_Shockwave_Accel="
                                            + wxString::Format("%d", acceleration).ToStdString()
                                            + ",E_SLIDER_Shockwave_CenterX=" + centerX
                                            + ",E_SLIDER_Shockwave_CenterY=" + centerY
                                            + ",E_SLIDER_Shockwave_End_Radius=" + wxString::Format("%d", endRadius).ToStdString()
                                            + ",E_SLIDER_Shockwave_End_Width=" + wxString::Format("%d", endWidth).ToStdString()
                                            + ",E_SLIDER_Shockwave_Start_Radius=" + wxString::Format("%d", startRadius).ToStdString()
                                            + ",E_SLIDER_Shockwave_Start_Width=" + wxString::Format("%d", startWidth).ToStdString()
                                            + blend_string;
                        layer->AddEffect(0, "Shockwave", settings, palette, startms, endms, false, false);
                    }
                    else if( type == "Fan" )
                    {
                        int revolutionsPerSecond = wxAtoi(element->GetAttribute("revolutionsPerSecond"));
                        int blades = wxAtoi(element->GetAttribute("blades"));
                        int blade_width = wxAtoi(element->GetAttribute("width"));
                        int elementAngle = wxAtoi(element->GetAttribute("elementAngle"));
                        int elementStepAngle = wxAtoi(element->GetAttribute("elementStepAngle"));
                        int numElements = elementAngle / elementStepAngle;
                        numElements = std::max(1, numElements);
                        numElements = std::min(numElements, 4);
                        blades = std::max(1, blades);
                        blades = std::min(blades, 16);
                        int tailms = int(double(endms-startms) * (blade_width * 2.0)/100.0) + 35;
                        endms += tailms;
                        int duration = 100 - int(tailms * 100.0 / (endms-startms));
                        std::string settings = "E_CHECKBOX_Fan_Reverse=" + wxString::Format("%d", startAngle > endAngle).ToStdString()
                                            + ",E_CHECKBOX_Fan_Blend_Edges=1"
                                            + ",E_NOTEBOOK_Fan=Position,E_SLIDER_Fan_Accel=" + wxString::Format("%d", acceleration).ToStdString()
                                            + ",E_SLIDER_Fan_Blade_Angle=" + wxString::Format("%d", elementAngle).ToStdString()
                                            + ",E_SLIDER_Fan_Blade_Width=" + wxString::Format("%d", blade_width).ToStdString()
                                            + ",E_SLIDER_Fan_CenterX=" + centerX
                                            + ",E_SLIDER_Fan_CenterY=" + centerY
                                            + ",E_SLIDER_Fan_Duration=" + wxString::Format("%d", duration).ToStdString()
                                            + ",E_SLIDER_Fan_Element_Width=" + wxString::Format("%d", 100).ToStdString()
                                            + ",E_SLIDER_Fan_Num_Blades=" + wxString::Format("%d", blades).ToStdString()
                                            + ",E_SLIDER_Fan_Num_Elements=" + wxString::Format("%d", numElements).ToStdString()
                                            + ",E_SLIDER_Fan_End_Radius=" + wxString::Format("%d", endRadius).ToStdString()
                                            + ",E_SLIDER_Fan_Revolutions=" + wxString::Format("%d", (int)((double)revolutionsPerSecond*((double)(endms-startms)/1000.0)*3.6)).ToStdString()
                                            + ",E_SLIDER_Fan_Start_Angle=" + wxString::Format("%d", startAngle).ToStdString()
                                            + ",E_SLIDER_Fan_Start_Radius=" + wxString::Format("%d", startRadius).ToStdString()
                                            + blend_string;
                        layer->AddEffect(0, "Fan", settings, palette, startms, endms, false, false);
                    }
                }
            }
        } else if ("scenes" == e->GetName()) {
            for(wxXmlNode* element=e->GetChildren(); element!=NULL; element=element->GetNext()) {
                if ("scene" == element->GetName()) {
                    wxString startms = element->GetAttribute("startCentisecond") + "0";
                    wxString endms = element->GetAttribute("endCentisecond") + "0";
                    wxString type = element->GetAttribute("type");
                    int layer_index = wxAtoi(element->GetAttribute("layer"));
                    xlColor startc = GetColor(element->GetAttribute("red1").ToStdString(),
                                             element->GetAttribute("green1").ToStdString(),
                                             element->GetAttribute("blue1").ToStdString());
                    xlColor endc = GetColor(element->GetAttribute("red2").ToStdString(),
                                             element->GetAttribute("green2").ToStdString(),
                                             element->GetAttribute("blue2").ToStdString());
                    while( model->GetEffectLayerCount() < layer_index ) {
                        model->AddEffectLayer();
                    }

                    int start_time = wxAtoi(startms);
                    int end_time = wxAtoi(endms);
                    layer = FindOpenLayer(model, layer_index, start_time, end_time, reserved);
                    if ("" == imagePfx) {
                        wxFileDialog fd(this,
                                        "Choose location and base name for image files",
                                        showDirectory,
                                        wxEmptyString,
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFD_SAVE);
                        while (fd.ShowModal() == wxID_CANCEL || fd.GetFilename() == "") {
                        }
                        imagePfx = fd.GetPath();
                    }

                    std::string ru = "0.0";
                    std::string rd = "0.0";
                    std::string imageName;
                    bool isFull = false;
                    wxRect rect;

                    bool isPartOfModel = IsPartOfModel(element, num_rows, num_columns, isFull, rect, reverse_xy);

                    if (isPartOfModel && isFull) {
                        //Every pixel in the model is specified, we can use a color wash or on instead of images


                        std::string palette = "C_BUTTON_Palette1=" + (std::string)startc + ",C_CHECKBOX_Palette1=1,C_BUTTON_Palette2="
                            + (std::string)endc
                            + ",C_CHECKBOX_Palette2=1,";


                        std::string settings = blend_string;
                        if (startc == endc) {
                            layer->AddEffect(0, "On", settings, palette, start_time, end_time, false, false);
                        } else if (startc == xlBLACK) {
                            std::string palette1 = "C_BUTTON_Palette1=" + (std::string)endc + ",C_CHECKBOX_Palette1=1,C_BUTTON_Palette2="
                                + (std::string)startc +
                                ",C_CHECKBOX_Palette2=1";
                            settings += ",E_TEXTCTRL_Eff_On_Start=0";
                            layer->AddEffect(0, "On", settings, palette1, start_time, end_time, false, false);
                        } else if (endc == xlBLACK) {
                            settings += ",E_TEXTCTRL_Eff_On_End=0";
                            layer->AddEffect(0, "On", "E_TEXTCTRL_Eff_On_End=0", palette, start_time, end_time, false, false);
                        } else {
                            layer->AddEffect(0, "Color Wash", settings, palette, start_time, end_time, false, false);
                        }
                    } else if (isPartOfModel && rect.x != -1) {
                        //forms a simple rectangle, we can use a ColorWash affect for this with a partial rectangle
                        std::string palette = "C_BUTTON_Palette1=" + (std::string)startc
                            + ",C_CHECKBOX_Palette1=1,C_BUTTON_Palette2=" + (std::string)endc
                            + ",C_CHECKBOX_Palette2=1";



                        std::string settings = "B_CUSTOM_SubBuffer=";
                        std::string val = wxString::Format("%d", rect.x).ToStdString();
                        if( !CalcBoundedPercentage(val, num_columns, false, x_offset) ) continue;
                        settings += val;
                        settings += "x";
                        val = wxString::Format("%d", rect.y);
                        if( !CalcBoundedPercentage(val, num_rows, reverse_rows ^ reverse_xy, y_offset) ) continue;
                        settings += val;
                        settings += "x";
                        val = wxString::Format("%d", rect.width);
                        if( !CalcBoundedPercentage(val, num_columns, false, x_offset) ) continue;
                        settings += val;
                        settings += "x";
                        val = wxString::Format("%d", rect.height);
                        if( !CalcBoundedPercentage(val, num_rows, reverse_rows ^ reverse_xy, y_offset) ) continue;
                        settings += val;
                        settings += blend_string;

                        layer->AddEffect(0, "Color Wash", settings, palette, start_time, end_time, false, false);
                    } else if (isPartOfModel) {
                        if (startc == xlBLACK || endc == xlBLACK || endc == startc) {
                            imageName = CreateSceneImage(imagePfx, "", element, num_columns, num_rows, false, reverse_xy,
                                                         (startc == xlBLACK) ? endc : startc, y_offset,
                                                         imageResizeType, modelSize);
                            wxString ramp = wxString::Format("%lf", (double)(end_time - start_time) / 1000.0);
                            if (endc == xlBLACK) {
                                rd = ramp;
                            }
                            if (startc == xlBLACK) {
                                ru = ramp;
                            }
                        } else {
                            int time = wxAtoi(endms) - wxAtoi(startms);
                            int numFrames = time / SeqData.FrameTime();
                            xlColor color;
                            for (int x = 0; x < numFrames; x++) {
                                double ratio = x;
                                ratio /= numFrames;
                                color.Set(ChannelBlend(startc.Red(),endc.Red(),ratio),
                                          ChannelBlend(startc.Green(),endc.Green(),ratio),
                                          ChannelBlend(startc.Blue(),endc.Blue(),ratio));
                                wxString s = CreateSceneImage(imagePfx, wxString::Format("-%d", x+1).ToStdString(),
                                                              element,
                                                              num_columns, num_rows, false, reverse_xy,
                                                              color, y_offset,
                                                              imageResizeType, modelSize);
                                if (x == 0) {
                                    imageName = s;
                                }
                            }
                        }

                        std::string settings = "E_CHECKBOX_Pictures_WrapX=0,E_CHOICE_Pictures_Direction=none,"
                            "E_SLIDER_PicturesXC=0"
                            ",E_SLIDER_PicturesYC=0"
                            ",E_CHECKBOX_Pictures_ScaleToFit=1"
                            ",E_SLIDER_Pictures_StartScale=100"
                            ",E_SLIDER_Pictures_EndScale=100"
                            ",E_CHECKBOX_Pictures_PixelOffsets=1"
                            ",E_TEXTCTRL_Pictures_Speed=1.0"
                            ",E_TEXTCTRL_Pictures_FrameRateAdj=1.0"
                            ",E_TEXTCTRL_Pictures_Filename=" + imageName;
                        if( type == "shimmer" ) {
                            settings += ",E_CHECKBOX_Pictures_Shimmer=1";
                        }
                        settings += blend_string;
                        if (ru != "0.0") {
                            settings += ",T_TEXTCTRL_Fadein=" + ru;
                        }
                        if (rd != "0.0") {
                            settings += ",T_TEXTCTRL_Fadeout=" + rd;
                        }
                        layer->AddEffect(0, "Pictures", settings, "", start_time, end_time, false, false);
                    }
                }
            }
        } else if ("textActions" == e->GetName()) {
            for(wxXmlNode* element=e->GetChildren(); element!=NULL; element=element->GetNext()) {
                if ("textAction" == element->GetName()) {
                    wxString startms = element->GetAttribute("startCentisecond") + "0";
                    wxString endms = element->GetAttribute("endCentisecond") + "0";
                    std::string text = element->GetAttribute("text").ToStdString();
                    wxString fontName = element->GetAttribute("fontName");
                    int fontSize = wxAtoi(element->GetAttribute("fontCapsHeight", "6"));
                    int fontCellWidth = wxAtoi(element->GetAttribute("fontCellWidth", "6"));
                    wxString mask = element->GetAttribute("maskType");

                    // SuperStar fonts are not as wide as they are listed.  This gets us closer to reality.
                    fontCellWidth = (fontCellWidth * 2) / 3;
                    fontSize += 4;

                    int rotation = wxAtoi(element->GetAttribute("rotation", "90"));
                    if( reverse_xy ) {
                        rotation -= 90;
                    }
                    //int direction = wxAtoi(element->GetAttribute("direction", "0"));
                    int xStart = wxAtoi(element->GetAttribute("xStart", "0"));
                    int yStart = wxAtoi(element->GetAttribute("yStart", "0"));
                    int xEnd = wxAtoi(element->GetAttribute("xEnd", "0"));
                    int yEnd = wxAtoi(element->GetAttribute("yEnd", "0"));
                    if( reverse_xy ) {
                        std::swap( xStart, yStart );
                        std::swap( xEnd, yEnd );
                    }

                    xlColor color = GetColor(element->GetAttribute("red").ToStdString(),
                                             element->GetAttribute("green").ToStdString(),
                                             element->GetAttribute("blue").ToStdString());

                    int layer_index = wxAtoi(element->GetAttribute("layer"));
                    while( model->GetEffectLayerCount() < layer_index ) {
                        model->AddEffectLayer();
                    }
                    int start_time = wxAtoi(startms);
                    int end_time = wxAtoi(endms);
                    layer = FindOpenLayer(model, layer_index, start_time, end_time, reserved);
                    int lorWidth = text.size() * fontCellWidth;
                    int lorHeight = fontSize;

                    std::string font = "arial " + wxString::Format("%s%d", (fontName.Contains("Bold") ? "bold " : ""), fontSize).ToStdString();
                    std::string eff = "normal";
                    if (fontName.Contains("Vertical")) {
                        eff = "vert text down";
                        lorWidth = fontCellWidth;
                        lorHeight = text.size() * fontSize;
                    } else if (rotation == 90) {
                        eff = "rotate down 90";
                        lorWidth = fontSize;
                        lorHeight = text.size() * fontCellWidth;
                    } else if (rotation == 270 || rotation == -90) {
                        eff = "rotate up 90";
                        lorWidth = fontSize;
                        lorHeight = text.size() * fontCellWidth;
                    }

                    // calculate everything off the LOR center
                    xStart += round((double)lorWidth / 2.0);
                    xEnd += round((double)lorWidth / 2.0);
                    yStart += round((double)lorHeight / 2.0);
                    yEnd += round((double)lorHeight / 2.0);

                    yStart = -yStart + num_rows;
                    yEnd = -yEnd + num_rows;

                    xStart = CalcUnBoundedPercentage(xStart, num_columns) - 50;
                    xEnd = CalcUnBoundedPercentage(xEnd, num_columns) - 50;
                    yStart = CalcUnBoundedPercentage(yStart, num_rows) - 50;
                    yEnd = CalcUnBoundedPercentage(yEnd, num_rows) - 50;

                    std::string palette = "C_BUTTON_Palette1=" + (std::string)color + ",C_CHECKBOX_Palette1=1"
                        + ",C_CHECKBOX_Palette2=0,C_CHECKBOX_Palette3=0,C_CHECKBOX_Palette4=0,C_CHECKBOX_Palette5=0"
                        + ",C_CHECKBOX_Palette6=0,C_CHECKBOX_Palette7=0,C_CHECKBOX_Palette8=0";
                    std::string settings =
                        "E_CHECKBOX_TextToCenter=0,E_TEXTCTRL_Text=" + text
                        + ",E_TEXTCTRL_Text_Speed=26,"
                        + "E_CHOICE_Text_Count=none,"
                        + "E_CHOICE_Text_Dir=vector,E_CHECKBOX_Text_PixelOffsets=0,"
                        + "E_CHOICE_Text_Effect=" + eff + ","
                        + "E_FONTPICKER_Text_Font=" + font + ","
                        + "E_SLIDER_Text_XStart=" + wxString::Format("%d", xStart).ToStdString() + ","
                        + "E_SLIDER_Text_YStart=" + wxString::Format("%d", yStart).ToStdString() + ","
                        + "E_SLIDER_Text_XEnd=" + wxString::Format("%d", xEnd).ToStdString() + ","
                        + "E_SLIDER_Text_YEnd=" + wxString::Format("%d", yEnd).ToStdString();
                    if( mask == "positiveMask" ) {
                        settings += ",T_CHOICE_LayerMethod=Normal";
                    } else if( mask == "negativeMask" ) {
                        settings += ",T_CHOICE_LayerMethod=1 is Mask";
                    } else {
                        settings += blend_string;
                    }

                    layer->AddEffect(0, "Text", settings, palette, start_time, end_time, false, false);
                }
            }

        } else if ("imageActions" == e->GetName()) {
            for(wxXmlNode* element=e->GetChildren(); element!=NULL; element=element->GetNext()) {
                if ("imageAction" == element->GetName()) {
                    //<imageAction name="Image Action 14" colorType="nativeColor" maskType="normal" rotation="0" direction="8"
                    //  stopAtEdge="0" layer="3" xStart="-1" yStart="0" xEnd="0" yEnd="0" startCentisecond="115" endCentisecond="145"
                    //  preRampTime="0" rampTime="0" fadeToBright="0" fadeFromBright="0" imageIndex="5" savedIndex="0">

                    int idx = wxAtoi(element->GetAttribute("imageIndex"));
                    int startms = wxAtoi(element->GetAttribute("startCentisecond")) * 10;
                    int endms = wxAtoi(element->GetAttribute("endCentisecond")) * 10;
                    int layer_index = wxAtoi(element->GetAttribute("layer"));
                    int rampDownTime = wxAtoi(element->GetAttribute("rampTime")) * 10;
                    int rampUpTime = wxAtoi(element->GetAttribute("preRampTime")) * 10;
                    while( model->GetEffectLayerCount() <= layer_index ) {
                        model->AddEffectLayer();
                    }
                    std::string rampUpTimeString = "0";
                    if (rampUpTime) {
                        double fadeIn = rampUpTime;
                        fadeIn /= 1000;  //FadeIn is in seconds
                        rampUpTimeString = wxString::Format("%lf", fadeIn);
                    }
                    std::string rampDownTimeString = "0";
                    if (rampDownTime) {
                        double fade = rampDownTime;
                        fade /= 1000;  //FadeIn is in seconds
                        rampDownTimeString = wxString::Format("%lf", fade);
                    }

                    int startx = wxAtoi(element->GetAttribute("xStart"));
                    int starty = wxAtoi(element->GetAttribute("yStart"));
                    int endx = wxAtoi(element->GetAttribute("xEnd"));
                    int endy = wxAtoi(element->GetAttribute("yEnd"));
                    if( reverse_xy ) {
                        std::swap(startx, starty);
                        std::swap(endx, endy);
                        starty = -starty;
                        endy = -endy;
                    }
                    ImageInfo &imgInfo = imageInfo[idx];
                    int x = imgInfo.xOffset;
                    int y = imgInfo.yOffset;

                    layer = FindOpenLayer(model, layer_index, startms, endms, reserved);
                    if (endy == starty && endx == startx) {
                        x += round((double)startx*imgInfo.scaleX);
                        y -= round((double)starty*imgInfo.scaleY);
                        std::string settings = "E_CHECKBOX_Pictures_WrapX=0,E_CHOICE_Pictures_Direction=none,"
                            "E_SLIDER_PicturesXC=" + wxString::Format("%d", x).ToStdString()
                            + ",E_SLIDER_PicturesYC=" + wxString::Format("%d", y).ToStdString()
                            + ",E_CHECKBOX_Pictures_ScaleToFit=0"
                            + ",E_SLIDER_Pictures_StartScale=100"
                            + ",E_SLIDER_Pictures_EndScale=100"
                            + ",E_CHECKBOX_Pictures_PixelOffsets=1"
                            + ",E_TEXTCTRL_Pictures_Filename=" + imgInfo.imageName
                            + ",E_TEXTCTRL_Pictures_Speed=1.0"
                            + ",E_TEXTCTRL_Pictures_FrameRateAdj=1.0";

                            if ("0" != rampUpTimeString) {
                                settings += ",T_TEXTCTRL_Fadein=" + rampUpTimeString;
                            }
                            if ("0" != rampDownTimeString) {
                                settings += ",T_TEXTCTRL_Fadeout=" + rampDownTimeString;
                            }
                            settings += blend_string;

                        layer->AddEffect(0, "Pictures", settings, "", startms, endms, false, false);
                    } else {
                        std::string settings = "E_CHECKBOX_Pictures_WrapX=0,E_CHOICE_Pictures_Direction=vector,"
                            "E_SLIDER_PicturesXC=" + wxString::Format("%d", x + (int)round((double)startx*imgInfo.scaleX)).ToStdString()
                            + ",E_SLIDER_PicturesYC=" + wxString::Format("%d", y - (int)round((double)starty*imgInfo.scaleY)).ToStdString()
                            + ",E_SLIDER_PicturesEndXC=" + wxString::Format("%d", x + (int)round((double)endx*imgInfo.scaleX)).ToStdString()
                            + ",E_SLIDER_PicturesEndYC=" + wxString::Format("%d", y - (int)round((double)endy*imgInfo.scaleY)).ToStdString()
                            + ",E_CHECKBOX_Pictures_ScaleToFit=0"
                            + ",E_SLIDER_Pictures_StartScale=100"
                            + ",E_SLIDER_Pictures_EndScale=100"
                            + ",E_TEXTCTRL_Pictures_Speed=1.0"
                            + ",E_TEXTCTRL_Pictures_FrameRateAdj=1.0"
                            + ",E_CHECKBOX_Pictures_PixelOffsets=1"
                            + ",E_TEXTCTRL_Pictures_Filename=" + imgInfo.imageName;

                        if ("0" != rampUpTimeString) {
                            settings += ",T_TEXTCTRL_Fadein=" + rampUpTimeString;
                        }
                        if ("0" != rampDownTimeString) {
                            settings += ",T_TEXTCTRL_Fadeout=" + rampDownTimeString;
                        }
                        settings += blend_string;

                        layer->AddEffect(0, "Pictures", settings, "", startms, endms, false, false);
                    }
                }
            }
        }
    }
    return true;
}

void AddLSPEffect(EffectLayer *layer, int pos, int epos, int in, int out, int eff, const wxColor &c, int bst, int ben) {
    if (eff == 4) {
        //off
        return;
    }
    xlColor color(c);
    xlColor color2(xlBLACK);

    bool isShimmer = eff == 5 || eff == 6;
    std::string effect = "On";
    std::string settings = wxString::Format("E_TEXTCTRL_Eff_On_End=%d,E_TEXTCTRL_Eff_On_Start=%d", out, in).ToStdString();
    if (isShimmer) {
        settings += ",E_CHECKBOX_On_Shimmer=1";
    }

    if (bst != 0 && ben != 0) {
        color = xlColor(bst & 0xFFFFFF, false);
        color2 = xlColor(ben & 0xFFFFFF, false);
        if (color == color2) {
            color2 = xlBLACK;
        } else {
            effect = "Color Wash";
            settings = isShimmer ? "E_CHECKBOX_ColorWash_Shimmer=1" : "";
        }
    }

    if (xlBLACK == color && xlBLACK == color2) {
        //nutcracker or other effects imported into LSP generate "BLACK" effects in the sequence.  Don't import them.
        return;
    }

    std::string palette = "C_BUTTON_Palette1=" + (std::string)color + ",C_CHECKBOX_Palette1=1,C_BUTTON_Palette2=" + (std::string)color2
        + ",C_CHECKBOX_Palette2=1";

    int start_time = (int)(pos * 50.0 / 4410.0);
    int end_time = (int)((epos - 1) * 50.0 / 4410.0);
    layer->AddEffect(0, effect, settings, palette, start_time, end_time, false, false);
}

void MapLSPEffects(EffectLayer *layer, wxXmlNode *node, const wxColor &c) {
    if (node == nullptr) {
        return;
    }
    int eff = -1;
    int in = 1, out = 1, pos = 1;

    int bst = 0, ben = 0;


    for (wxXmlNode *cnd = node->GetChildren(); cnd != nullptr; cnd = cnd->GetNext()) {
        if (cnd->GetName() == "Tracks") {
            for (wxXmlNode *cnnd = cnd->GetChildren(); cnnd != nullptr; cnnd = cnnd->GetNext()) {
                if (cnnd->GetName() == "Track") {
                    for (wxXmlNode *ind = cnnd->GetChildren(); ind != nullptr; ind = ind->GetNext()) {
                        if (ind->GetName() == "Intervals") {
                            for (wxXmlNode *ti = ind->GetChildren(); ti != nullptr; ti = ti->GetNext()) {
                                if (ti->GetName() == "TimeInterval") {
                                    int neff = wxAtoi(ti->GetAttribute("eff", "4"));
                                    if (eff != -1 && neff != 7) {
                                        int npos = wxAtoi(ti->GetAttribute("pos", "1"));
                                        AddLSPEffect(layer, pos, npos, in, out, eff, c, bst, ben);
                                    }
                                    if (neff != 7) {
                                        pos = wxAtoi(ti->GetAttribute("pos", "1"));
                                        eff = neff;
                                        in = wxAtoi(ti->GetAttribute("in", "1"));
                                        out = wxAtoi(ti->GetAttribute("out", "1"));
                                        bst = wxAtoi(ti->GetAttribute("bst", "0"));
                                        ben = wxAtoi(ti->GetAttribute("ben", "0"));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void MapLSPStrand(StrandElement *layer, wxXmlNode *node, const wxColor &c) {
    int nodeNum = 0;
    for (wxXmlNode *nd = node->GetChildren(); nd != nullptr; nd = nd->GetNext()) {
        if (nd->GetName() == "Channels") {
            for (wxXmlNode *cnd = nd->GetChildren(); cnd != nullptr; cnd = cnd->GetNext()) {
                if (cnd->GetName() == "Channel") {
                    EffectLayer *el = layer->GetNodeLayer(nodeNum, true);
                    MapLSPEffects(el, cnd, c);
                    nodeNum++;
                    if (nodeNum >= layer->GetNodeLayerCount()) {
                        return;
                    }
                }
            }
        }
    }
}

void xLightsFrame::ImportLSP(const wxFileName &filename) {
    wxStopWatch sw; // start a stopwatch timer

    LMSImportChannelMapDialog dlg(this, filename);
    dlg.mSequenceElements = &mSequenceElements;
    dlg.xlights = this;


    wxFileName msq_file(filename);
    wxString msq_doc = msq_file.GetFullPath();
    wxFileInputStream fin(msq_doc);
    wxZipInputStream zin(fin);
    wxZipEntry *ent = zin.GetNextEntry();


    wxXmlDocument seq_xml;
    std::map<wxString, wxXmlDocument> cont_xml;
    std::map<wxString, wxXmlNode *> nodes;
    std::map<wxString, wxXmlNode *> strandNodes;

    while (ent != nullptr) {
        if (ent->GetName() == "Sequence") {
            seq_xml.Load(zin);
        } else {
            std::string id("1");
            wxXmlDocument &doc =  cont_xml[ent->GetName()];

            if (doc.Load(zin)) {
                for (wxXmlNode *nd = doc.GetRoot()->GetChildren(); nd != nullptr; nd = nd->GetNext()) {
                    if (nd->GetName() == "ControllerName") {
                        id = nd->GetChildren()->GetContent();
                    }
                }
                strandNodes[id] = doc.GetRoot();
                dlg.ccrNames.push_back(id);
                for (wxXmlNode *nd = doc.GetRoot()->GetChildren(); nd != nullptr; nd = nd->GetNext()) {
                    if (nd->GetName() == "Channels") {
                        for (wxXmlNode *cnd = nd->GetChildren(); cnd != nullptr; cnd = cnd->GetNext()) {
                            if (cnd->GetName() == "Channel") {
                                std::string cname;
                                for (wxXmlNode *cnnd = cnd->GetChildren(); cnnd != nullptr; cnnd = cnnd->GetNext()) {
                                    if (cnnd->GetName() == "Tracks") {
                                        for (wxXmlNode *tnd = cnnd->GetChildren(); tnd != nullptr; tnd = tnd->GetNext()) {
                                            if (tnd->GetName() == "Track") {
                                                for (wxXmlNode *tnd2 = tnd->GetChildren(); tnd2 != nullptr; tnd2 = tnd2->GetNext()) {
                                                    if (tnd2->GetName() == "Name") {
                                                        cname = tnd2->GetChildren()->GetContent();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                cname = id + "/" + cname;
                                nodes[cname] = cnd;
                                dlg.channelNames.push_back(cname);
                                dlg.channelColors[cname] = xlWHITE;
                            }
                        }
                    }
                }
            } else {
                static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
                logger_base.warn("Could not parse XML file %s.", (const char *)ent->GetName().c_str());
                wxLogError("Could not parse XML file %s", ent->GetName().c_str());
            }
        }
        ent = zin.GetNextEntry();
    }

    std::sort(dlg.channelNames.begin(), dlg.channelNames.end());
    dlg.channelNames.insert(dlg.channelNames.begin(), "");
    std::sort(dlg.ccrNames.begin(), dlg.ccrNames.end());
    dlg.ccrNames.insert(dlg.ccrNames.begin(), "");

    dlg.Init();

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    int row = 0;
    for (size_t m = 0; m < dlg.modelNames.size(); m++) {
        std::string modelName = dlg.modelNames[m];
        Model *mc = GetModel(modelName);
        ModelElement * model = nullptr;
        for (size_t i=0;i<mSequenceElements.GetElementCount();i++) {
            if (mSequenceElements.GetElement(i)->GetType() == ELEMENT_TYPE_MODEL
                && modelName == mSequenceElements.GetElement(i)->GetName()) {
                model = dynamic_cast<ModelElement*>(mSequenceElements.GetElement(i));
            }
        }
        if (dlg.ChannelMapGrid->GetCellValue(row, 3) != "" && !dlg.MapByStrand->IsChecked()) {
            MapLSPEffects(model->GetEffectLayer(0), nodes[dlg.ChannelMapGrid->GetCellValue(row, 3)],
                          dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4));
        }
        row++;

        if (!dlg.MapByStrand->IsChecked()) {
            for (int str = 0; str < mc->GetNumSubModels(); str++) {
                SubModelElement *se = model->GetSubModel(str);

                if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                    MapLSPEffects(se->GetEffectLayer(0), nodes[dlg.ChannelMapGrid->GetCellValue(row, 3)],
                                  dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4));
                }
                row++;
            }
        }
        for (int str = 0; str < mc->GetNumStrands(); str++) {
            StrandElement *se = model->GetStrand(str, true);

            if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                if (dlg.MapByStrand->IsChecked()) {
                    MapLSPStrand(se, strandNodes[dlg.ChannelMapGrid->GetCellValue(row, 3)],
                                  dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4));
                } else {
                    MapLSPEffects(se->GetEffectLayer(0), nodes[dlg.ChannelMapGrid->GetCellValue(row, 3)],
                                  dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4));
                }
            }
            row++;
            if (!dlg.MapByStrand->IsChecked()) {
                for (int n = 0; n < mc->GetStrandLength(str); n++) {
                    if ("" != dlg.ChannelMapGrid->GetCellValue(row, 3)) {
                        NodeLayer *nl = se->GetNodeLayer(n, true);
                        MapLSPEffects(nl, nodes[dlg.ChannelMapGrid->GetCellValue(row, 3)],
                                      dlg.ChannelMapGrid->GetCellBackgroundColour(row, 4));
                    }
                    row++;
                }
            }
        }
    }

    float elapsedTime = sw.Time()/1000.0; //msec => sec
    SetStatusText(wxString::Format("'%s' imported in %4.3f sec.", filename.GetPath(), elapsedTime));
}

