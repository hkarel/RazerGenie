// Copyright (C) 2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LEDWIDGET_H
#define LEDWIDGET_H

#include <QWidget>
#include <libopenrazer.h>

class LedWidget : public QWidget
{
    Q_OBJECT
public:
    LedWidget(QWidget *parent, libopenrazer::Led *led);
    libopenrazer::Led *mLed;
    libopenrazer::Led *led();

    // Color buttons
    void colorButtonClicked();
    // Effect comboboxes
    void fxComboboxChanged(int index);
    // Brightness sliders
    void brightnessSliderChanged(int value);
    // Wave buttons
    void waveRadioButtonChanged(bool enabled);

    openrazer::RGB getColorForButton(int num);
    openrazer::WaveDirection getWaveDirection();

    void applyEffect();
    void applyEffectStandardLoc(openrazer::RazerEffect identifier);
};

#endif // LEDWIDGET_H
