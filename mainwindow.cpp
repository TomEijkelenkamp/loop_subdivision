#include "mainwindow.h"

#include "initialization/meshinitializer.h"
#include "initialization/objfile.h"
#include "subdivision/loopsubdivider.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow Creates a new Main Window UI.
 * @param parent Qt parent widget.
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // UI enable / disable
  ui->MeshGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
  ui->IsoGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
  ui->MainDisplay->settings.isoFrequency = ui->IsoSpinBox->value();
  ui->IsoSpinBox->setEnabled(ui->IsoCheckBox->isChecked());
  ui->IsoFrequencyLabel->setEnabled(ui->IsoCheckBox->isChecked());

  // Create a palette for disabling isogroupbox title
  QPalette palette;
  palette.setColor(QPalette::Disabled, QPalette::WindowText,
                   QApplication::palette().color(QPalette::Disabled, QPalette::WindowText));
  ui->IsoGroupBox->setPalette(palette);
}

/**
 * @brief MainWindow::~MainWindow Deconstructs the main window.
 */
MainWindow::~MainWindow() {
  delete ui;

  meshes.clear();
  meshes.squeeze();
}

/**
 * @brief MainWindow::importOBJ Imports an obj file and adds the constructed
 * half-edge to the collection of meshes.
 * @param fileName Path of the .obj file.
 */
void MainWindow::importOBJ(const QString& fileName) {
  OBJFile newModel = OBJFile(fileName);
  meshes.clear();
  meshes.squeeze();

  if (newModel.loadedSuccessfully()) {
    MeshInitializer meshInitializer;
    meshes.append(meshInitializer.constructHalfEdgeMesh(newModel));
    ui->MainDisplay->updateBuffers(meshes[0]);
    ui->MainDisplay->settings.modelLoaded = true;
  } else {
    ui->MainDisplay->settings.modelLoaded = false;
  }

  ui->MeshGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
  ui->IsoGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
  ui->SubdivSteps->setValue(0);
  ui->MainDisplay->update();
}

// Don't worry about adding documentation for the UI-related functions.

void MainWindow::on_LoadOBJ_pressed() {
  QString filename = QFileDialog::getOpenFileName(
      this, "Import OBJ File", "../", tr("Obj Files (*.obj)"));
  importOBJ(filename);
}

void MainWindow::on_MeshPresetComboBox_currentTextChanged(
    const QString& meshName) {
  importOBJ(":/models/" + meshName + ".obj");
}

void MainWindow::on_SubdivSteps_valueChanged(int value) {
  Subdivider* subdivider = new LoopSubdivider();
  for (int k = meshes.size() - 1; k < value; k++) {
    meshes.append(subdivider->subdivide(meshes[k]));
  }
  ui->MainDisplay->updateBuffers(meshes[value]);
  delete subdivider;
}

void MainWindow::on_IsoCheckBox_toggled(bool checked) {
  if (checked) {
    ui->MainDisplay->settings.currentShader = ISO;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
  } else {
    ui->MainDisplay->settings.currentShader = PHONG;
  }
  ui->IsoSpinBox->setEnabled(checked);
  ui->IsoFrequencyLabel->setEnabled(checked);
  ui->MainDisplay->settings.wireframeMode = false;
  ui->MainDisplay->update();
}

void MainWindow::on_IsoSpinBox_valueChanged(int value) {
  ui->MainDisplay->settings.isoFrequency = value;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}
