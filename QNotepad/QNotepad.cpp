#include "QNotepad.h"
#include<QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDateTime>
#include <QTextDocument>

static const QString titleSuffix = "-���±�";
static const QString defaultTitle = "�ޱ���";
QNotepad::QNotepad(QWidget *parent)
	: QMainWindow(parent)
	, mSettings("./Setting.ini", QSettings::IniFormat)
	,mFindDlg(this)
{
	ui.setupUi(this);
	setWindowTitle(defaultTitle + titleSuffix);
	mChanged = false;
	mCurrentCursor.setPosition(0);
	InitShortcut();
	InitConnect();
}

void QNotepad::UpdateFileStatus()
{
	QString text = ui.plainTextEdit->toPlainText();
	mChanged = (!text.isEmpty());
}

void QNotepad::NewFile()
{
	if (mChanged)
	{
		if (!PreCheck())
		{
			return;
		}
	}
	mChanged = false;
	ui.plainTextEdit->clear();
	mCurrentFilePath.clear();
	setWindowTitle(defaultTitle + titleSuffix);
}

void QNotepad::OpenFile()
{
	if (mChanged)
	{
		if (!PreCheck())
		{
			return;
		}
	}
	QString lastPath = mSettings.value("LastFilePath").toString();
	if (lastPath.isEmpty())
	{
		lastPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	}
	mCurrentFilePath = QFileDialog::getOpenFileName(this,
		tr("��"),
		lastPath, 
		tr("�ı��ĵ�(*.txt);;�����ļ�(*.*)"));
	if (mCurrentFilePath.isEmpty())
	{
		return;
	}
	QFile file(mCurrentFilePath);
	if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		return;
	}
	QByteArray text = std::move(file.readAll());
	ui.plainTextEdit->setPlainText(QString(text));
	file.close();
	mChanged = false;
	QFileInfo fileInfo(mCurrentFilePath);
	setWindowTitle(fileInfo.baseName() + titleSuffix);
	mSettings.setValue("LastFilePath", fileInfo.absolutePath());
}

bool QNotepad::SaveFile()
{
	if (mCurrentFilePath.isEmpty())
	{
		QString lastPath = mSettings.value("LastFilePath").toString();
		if (lastPath.isEmpty())
		{
			lastPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		}
		mCurrentFilePath = QFileDialog::getSaveFileName(this,
			tr("���Ϊ"),
			lastPath,
			tr("�ı��ĵ�(*.txt);;�����ļ�(*.*)"));
		if (mCurrentFilePath.isEmpty())
		{
			return false;
		}
	}
	QFile file(mCurrentFilePath);
	if (!file.open(QIODevice::Text | QIODevice::WriteOnly))
	{
		SaveAsFile();
		return false;
	}
	file.write(ui.plainTextEdit->toPlainText().toStdString().c_str());
	file.close();
	mChanged = false;
	return true;
}

void QNotepad::SaveAsFile()
{
	QString oldFilePath = mCurrentFilePath;
	mCurrentFilePath.clear();
	if (SaveFile())
	{
		QFileInfo fileInfo(mCurrentFilePath);
		setWindowTitle(fileInfo.baseName() + titleSuffix);
		mSettings.setValue("LastFilePath", fileInfo.absolutePath());
	}
	else
	{
		mCurrentFilePath = oldFilePath;
	}
}

void QNotepad::InsertSysDataTime()
{
	QDateTime curDateTime = QDateTime::currentDateTime();
	QString strDateTime = curDateTime.toString(Qt::DateFormat::DefaultLocaleShortDate);
	ui.plainTextEdit->textCursor().insertText(strDateTime);
}

void QNotepad::Find()
{
	mFindDlg.show();
}

void QNotepad::FindNext()
{
	const QString& keyword = mFindDlg.GetKeyword();
	if (keyword.isEmpty())
	{
		return;
	}
	QTextDocument* doc = ui.plainTextEdit->document();
	QTextDocument::FindFlags options;
	if (mFindDlg.IsCaseSensitive())
	{
		options |= QTextDocument::FindCaseSensitively;
	}
	if (mFindDlg.isFindBackward())
	{
		options |= QTextDocument::FindBackward;
	}
	if (mFindDlg.IsWholeWord())
	{
		options |= QTextDocument::FindWholeWords;
	}
	mCurrentCursor = doc->find(mFindDlg.GetKeyword(), mCurrentCursor, options);
	if (!mCurrentCursor.isNull())
	{
		ui.plainTextEdit->setTextCursor(mCurrentCursor);
// 		QList<QTextEdit::ExtraSelection> extraSelections;//�ṩһ�ַ�ʽ��ʾѡ����ı�
// 		//extraSelections = ui.plainTextEdit->extraSelections();//��ȡ֮ǰ����������
// 		QTextEdit::ExtraSelection selection;
// 		//selection.format.setBackground(QColor(255,255,0));
// 		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
// 		selection.cursor = mCurrentCursor;
// 		selection.cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,mFindDlg.GetKeyword().size());//����ƶ���ĳһ�С��˴��ƶ�����һ�У���һ�н�������
// 		extraSelections.append(selection);
// 		ui.plainTextEdit->setExtraSelections(extraSelections);//���ø���
	}
	
}

void QNotepad::InitShortcut()
{
	//�ļ�
	ui.actionSave->setShortcut(tr("ctrl+S"));
	ui.actionNew->setShortcut(QKeySequence::New);
	ui.actionOpen->setShortcut(QKeySequence::Open);
	//�༭
	ui.actionUndo->setShortcut(QKeySequence::Undo);
	ui.actionCut->setShortcut(QKeySequence::Cut);
	ui.actionCopy->setShortcut(QKeySequence::Copy);
	ui.actionPaste->setShortcut(QKeySequence::Paste);
	ui.actionDelete->setShortcut(QKeySequence::Delete);
	ui.actionFind->setShortcut(QKeySequence::Find);
	ui.actionFindNext->setShortcut(QKeySequence::FindNext);
	ui.actionReplace->setShortcut(QKeySequence::Replace);
	ui.actionGo->setShortcut(tr("ctrl+G"));
	ui.actionAll->setShortcut(QKeySequence::SelectAll);
	ui.actionDate->setShortcut(tr("F5"));
}

void QNotepad::InitConnect()
{
	//�ļ�
	connect(ui.plainTextEdit, SIGNAL(textChanged()), this, SLOT(UpdateFileStatus()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(NewFile()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(SaveFile()));
	connect(ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	//�༭
	connect(ui.actionUndo, SIGNAL(triggered()), ui.plainTextEdit, SLOT(undo()));
	connect(ui.actionCut, SIGNAL(triggered()), ui.plainTextEdit, SLOT(cut()));
	connect(ui.actionCopy, SIGNAL(triggered()), ui.plainTextEdit, SLOT(copy()));
	connect(ui.actionPaste, SIGNAL(triggered()), ui.plainTextEdit, SLOT(paste()));
	//connect(ui.actionDelete, SIGNAL(triggered()), ui.plainTextEdit, SLOT(clear()));
	connect(ui.actionFind, SIGNAL(triggered()), this, SLOT(Find()));
	connect(ui.actionFindNext, SIGNAL(triggered()), this, SLOT(FindNext()));
	connect(ui.actionAll, SIGNAL(triggered()), ui.plainTextEdit, SLOT(selectAll()));
	connect(ui.actionDate, SIGNAL(triggered()), this, SLOT(InsertSysDataTime()));
}

bool QNotepad::PreCheck()
{
	QString promptInfo("\n" + mCurrentFilePath);
	if (mCurrentFilePath.isEmpty())
	{
		promptInfo = " " + defaultTitle;
	}
	QString text = QString(tr("�Ƿ񽫸��ı��浽%1?")).arg(promptInfo);
	int ret = QMessageBox::warning(this, tr("���±�"), text, tr("����"), tr("������"), tr("ȡ��"));
	if (ret == 0)
	{
		if (!SaveFile())
		{
			return false;
		}
	}
	else if (ret == 2)
	{
		return false;
	}
	return true;
}
