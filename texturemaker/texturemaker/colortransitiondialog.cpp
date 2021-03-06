/*
 * Copyright 2013 Miloslav Číž
 *
 * This file is part of PT Designer.
 *
 * PT Designer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PT Designer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with these files. If not, see <http://www.gnu.org/licenses/>.
 */

#include "colortransitiondialog.h"
#include "ui_colortransitiondialog.h"

//-----------------------------------------------------

ColorTransitionDialog::ColorTransitionDialog(c_block *block, QWidget *parent) :
    CustomBlockDialog(block,parent),
    ui(new Ui::ColorTransitionDialog)

{
  ui->setupUi(this);
  this->set_window_properties();

  color_transition_init(&this->transition);
  ui->transition_preview->set_transition(&this->transition);
  color_transition_from_string(&this->transition,(char *) this->block->get_parameters()->get_string_value("data").c_str());
  this->update_items();
}

//-----------------------------------------------------

ColorTransitionDialog::~ColorTransitionDialog()

{
  delete ui;
  color_transition_destroy(&this->transition);
}

//-----------------------------------------------------

int ColorTransitionDialog::selected_point()

{
  QString helper;
  QList<QListWidgetItem *> list;

  list = ui->points->selectedItems();

  if (list.length() == 0)
    return -1;

  helper = list.at(0)->text();

  return helper.left(helper.indexOf(':')).toInt();
}

//-----------------------------------------------------

void ColorTransitionDialog::update_items()

{
  unsigned int i;

  ui->points->clear();

  for (i = 0; (int) i < this->transition.number_of_points; i++)
    ui->points->addItem(
      QString::number(transition.points[i].value) + ": [" +
      QString::number(transition.points[i].red) + "," +
      QString::number(transition.points[i].green) + "," +
      QString::number(transition.points[i].blue) + "]");
}

//-----------------------------------------------------

void ColorTransitionDialog::on_delete_button_clicked()

{
  int index;

  index = this->selected_point();

  if (index < 0)
    return;

  color_transition_remove_point(index,&this->transition);
  this->update_items();
  ui->transition_preview->update();
}

//-----------------------------------------------------

void ColorTransitionDialog::on_add_button_clicked()

{
  color_transition_add_point(ui->coordination->value(),255,255,255,&this->transition);
  this->update_items();
  ui->transition_preview->update();
}

//-----------------------------------------------------

void ColorTransitionDialog::on_edit_button_clicked()

{
  QColorDialog dialog;
  int index;
  unsigned char r,g,b;

  index = this->selected_point();

  if (index < 0)
    return;

  color_transition_get_color(index,&r,&g,&b,&this->transition);

  dialog.setCurrentColor(QColor::fromRgb(r,g,b));

  if (dialog.exec() == QDialog::Accepted)
    {
      color_transition_remove_point(index,&this->transition);
      r = dialog.currentColor().red();
      g = dialog.currentColor().green();
      b = dialog.currentColor().blue();

      color_transition_add_point(index,r,g,b,&this->transition);
    }

  this->update_items();
  ui->transition_preview->update();
}

//-----------------------------------------------------

void ColorTransitionDialog::on_buttonBox_accepted()

{
  char buffer[1000];

  color_transition_to_string(&this->transition,buffer,1000);
  this->block->get_parameters()->set_string_value("data",buffer);
}

//-----------------------------------------------------

void ColorTransitionDialog::on_change_position_clicked()

{
  unsigned char r,g,b;
  int index;

  index = this->selected_point();

  if (index < 0)
    return;        // no point selected

  color_transition_get_color(index,&r,&g,&b,&this->transition);

  color_transition_remove_point(index,&this->transition);
  color_transition_add_point(ui->coordination->value(),r,g,b,&this->transition);

  this->update_items();
  ui->transition_preview->update();
}

//-----------------------------------------------------
