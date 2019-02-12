#include "confirm-deletion.hpp"
#include <QMessageBox>

bool confirmDeletion() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(nullptr, "Confirm", "Are you sure you want to delete?",
                                QMessageBox::Yes|QMessageBox::No);
	return (reply == QMessageBox::Yes);
}