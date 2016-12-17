#ifndef FPPCONNECTDIALOG_H
#define FPPCONNECTDIALOG_H

#include <wx/progdlg.h>

//(*Headers(FPPConnectDialog)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checklst.h>
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include <wx/xml/xml.h>

class FPPConnectDialog: public wxDialog
{
    void LoadSequencesFromFolder(wxString dir);
    void LoadSequences();
    bool CopyFile(std::string source, std::string target, bool backup, wxProgressDialog &progress, int start, int end);
    bool DoCopyFile(const std::string& source, const std::string& target, wxProgressDialog &progress, int start, int end);
    void ValidateWindow();
    bool IsValidIP(wxString ip);
    bool FTPUpload();
    bool USBUpload();
    void OnPopup(wxCommandEvent &event);

	public:

		FPPConnectDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~FPPConnectDialog();

		//(*Declarations(FPPConnectDialog)
		wxNotebook* Notebook_FPP;
		wxCheckBox* CheckBox_UploadModels;
		wxButton* Button_Upload;
		wxStaticText* StaticText2;
		wxPanel* Panel_USB;
		wxChoice* Choice_Drives;
		wxDirPickerCtrl* DirPickerCtrl_FPPMedia;
		wxButton* Button_Console;
		wxTextCtrl* TextCtrl_IPAddress;
		wxStaticText* StaticText1;
		wxCheckBox* CheckBox_UploadController;
		wxStaticText* StaticText3;
		wxStaticText* StaticText5;
		wxCheckListBox* CheckListBox_Sequences;
		wxPanel* Panel_FTP;
		wxStaticText* StaticText4;
		wxTextCtrl* TextCtrl_Password;
		wxTextCtrl* TextCtr_Username;
		//*)

	protected:

		//(*Identifiers(FPPConnectDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL_IPAddress;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL_Username;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL_Password;
		static const long ID_BUTTON_Console;
		static const long ID_PANEL_FTP;
		static const long ID_STATICTEXT4;
		static const long ID_CHOICE_Drives;
		static const long ID_STATICTEXT5;
		static const long ID_DIRPICKERCTRL1;
		static const long ID_PANEL_USB;
		static const long ID_NOTEBOOK_FPP;
		static const long ID_CHECKBOX_UploadController;
		static const long ID_CHECKBOX1;
		static const long ID_CHECKLISTBOX_Sequences;
		static const long ID_BUTTON_Upload;
		//*)

        static const long ID_MNU_SELECTALL;
        static const long ID_MNU_SELECTNONE;

	private:

		//(*Handlers(FPPConnectDialog)
		void OnButton_UploadClick(wxCommandEvent& event);
		void OnCheckListBox_SequencesToggled(wxCommandEvent& event);
		void OnCheckBox_UploadControllerClick(wxCommandEvent& event);
		void OnTextCtrl_IPAddressText(wxCommandEvent& event);
		void OnTextCtr_UsernameText(wxCommandEvent& event);
		void OnTextCtrl_PasswordText(wxCommandEvent& event);
		void OnNotebook_FPPPageChanged(wxNotebookEvent& event);
		void OnButton_ConsoleClick(wxCommandEvent& event);
		void OnFilePickerCtrl_MediaFolderFileChanged(wxFileDirPickerEvent& event);
		//*)

        void OnSequenceRClick(wxMouseEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
