#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

#include "mesh/mesh.h"
#include "subdivision/subdivider.h"

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class represents the main window.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 private slots:
  void on_LoadOBJ_pressed();
  void on_MeshPresetComboBox_currentTextChanged(const QString &meshName);
  void on_SubdivSteps_valueChanged(int value);
  void on_SubdivisionShadingCheckBox_toggled(bool checked);
  void on_ShadingRadioPhong_toggled(bool checked);
  void on_ShadingRadioNormal_toggled(bool checked);
  void on_ShadingRadioIsophote_toggled(bool checked);
  void on_blendNormalsBox_toggled(bool checked);
  void on_IsoSpinBox_valueChanged(int value);

 private:
  void importOBJ(const QString &fileName);

  Ui::MainWindow *ui;
  Subdivider *subdivider;
  QVector<Mesh> meshes;
};

#endif  // MAINWINDOW_H
