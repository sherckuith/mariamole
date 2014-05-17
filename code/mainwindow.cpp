#include "mainwindow.h"

//-----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);		
	
	//connect ( this, SIGNAL ( ShowErrorSignal ( QString ) ), SLOT ( ShowError ( QString  ) ) );
	connect (ui.actionSelect_workspace, SIGNAL(triggered()), this, SLOT(SetWorkspace));

	config.Load();	

	SetWorkspace();
}

//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{

}

//-----------------------------------------------------------------------------

void MainWindow::resizeEvent(QResizeEvent *event)
{
	float w = width();
	float h = height();
	//ui.tree->resize(0.1 * w, ui.tree->height());
	ui.messageTabs->resize(w, 0.1 * h);
	ui.editorTabs->resize(0.90 * w, h * 9);
	
}

//-----------------------------------------------------------------------------

void MainWindow::SetWorkspace(void)
{
	ui.tree->clear();

	QTreeWidgetItem *root = new QTreeWidgetItem(ui.tree);
	root->setText(0, tr("Workspace"));

//	QTreeWidgetItem item;
//	item.setText(0, "Workspace");
	//ui.tree->addTopLevelItem(&item);
	workspace.Open("C:/projetos/arduino/workspaces/mainframe-kaboom/arduino");
	AdjustWorkspaceTree();
}

//-----------------------------------------------------------------------------

void MainWindow::AdjustWorkspaceTree(void)
{
	QTreeWidgetItem *root = ui.tree->topLevelItem(0);

	// Fisr, find projects on workspace that are not yet on the tree, and add them.	
	for (int pwi=0; pwi < workspace.projects.size(); pwi++) {
		bool foundAtTree = false;
		for (int pti=0; pti < root->childCount(); pti++) {
			QTreeWidgetItem *projNode = root->child(pti);
			if (workspace.projects.at(pwi).name == projNode->text(0)) {
				foundAtTree = true;
				break;
			}
		}
		if (foundAtTree == false) {
			QTreeWidgetItem *projNode = new QTreeWidgetItem(root);
			projNode->setText(0, workspace.projects.at(pwi).name);			
			projNode = new QTreeWidgetItem(projNode);
			projNode->setText(0, "Extern files");
		}
	}
	
	// Then Update the tree with all projects on workspace
	int pti = 0;
	while (pti < root->childCount()) {
		QTreeWidgetItem *projNode = root->child(pti);		
	//for (int pti=0; pti < root->childCount(); pti++) {		
		bool foundAtWorkspace = false;
		for (int pwi=0; pwi < workspace.projects.size(); pwi++) {
			if (workspace.projects.at(pwi).name == projNode->text(0)) {
				foundAtWorkspace = true;
				AdjustProjectFilesOnTree(pwi, projNode);				
			}
		}
		// If project no longer exists on workspace, remove it from the tree
		if (foundAtWorkspace == false) {
			delete projNode;
			//ui.tree->removeItemWidget(projNode, 0);
		} else {
			pti++;
		}
	}	

	ui.tree->expandItem(root);
}

//-----------------------------------------------------------------------------

void MainWindow::AdjustProjectFilesOnTree(int pwi, QTreeWidgetItem * projNode)
{	
	vector <Project>::iterator project = workspace.projects.begin() + pwi;
	
	// Get the node with the label "External files"
	QTreeWidgetItem * externNode = NULL;
	for (int fti=0; fti < projNode->childCount(); fti++) {			
		if (projNode->child(fti)->text(0) != "External files") {
			externNode = projNode->child(fti);
		}		
	}

	// First, check if all files in this project at workspace exist at the tree. If not, add them to the tree
	for (int fwi=0; fwi < project->files.size(); fwi++) {
		bool foundAtTree = false;
		if (project->files.at(fwi).type == ptSource) {
			// source files
			for (int fti=0; fti < projNode->childCount(); fti++) {
				QTreeWidgetItem *fileNode = projNode->child(fti);
				if (project->files.at(fti).name == projNode->text(0)) {
					foundAtTree = true;
					break;
				}
			}
			if (foundAtTree == false) {			
				QTreeWidgetItem *fileNode = new QTreeWidgetItem(projNode);
				QString text = project->files.at(fwi).name;
				fileNode->setText(0, text);
			}
		} else { 
			// external files
			for (int fti=0; fti < externNode->childCount(); fti++) {
				QTreeWidgetItem *fileNode = externNode->child(fti);
				if (project->files.at(fti).name == externNode->text(0)) {
					foundAtTree = true;
					break;
				}
			}
			if (foundAtTree == false) {			
				QTreeWidgetItem *fileNode = new QTreeWidgetItem(externNode);
				QString text = project->files.at(fwi).name;
				fileNode->setText(0, text);
			}
		}		
	}

	// Now check if any file at the tree no longer exist on the workspace
	// In these cases, remove the file from the tree

	// source files
	int fti = 0;
	while (fti < projNode->childCount()) {
		QTreeWidgetItem *fileNode = projNode->child(fti);
		if (fileNode->text(0) == "Extern files") {
			fti++;
			continue;
		}

		bool foundAtWorkspace = false;
		for (int fwi=0; fwi < project->files.size(); fwi++) {
			if (project->files.at(fwi).type != ptSource) {
				continue;
			}		
			if (project->files.at(fwi).name == fileNode->text(0)) {
				foundAtWorkspace = true;
				break;
			}
		}
		if (foundAtWorkspace == false) {
			delete fileNode;
			//ui.tree->removeItemWidget(fileNode, 0);
		} else {
			fti++;
		}
	}

	// extern files
	fti = 0;
	while (fti < externNode->childCount()) {
		QTreeWidgetItem *fileNode = externNode->child(fti);
		QString nodeText = fileNode->text(0);
		
		bool foundAtWorkspace = false;
		for (int fwi=0; fwi < project->files.size(); fwi++) {
			if (project->files.at(fwi).type == ptSource) {
				continue;
			}		
			if (project->files.at(fwi).name == nodeText) {
				foundAtWorkspace = true;
				break;
			}
		}
		if (foundAtWorkspace == false) {
			delete fileNode;
			//ui.tree->removeItemWidget(fileNode, 0);
		} else {
			fti++;
		}
	}
}


//-----------------------------------------------------------------------------