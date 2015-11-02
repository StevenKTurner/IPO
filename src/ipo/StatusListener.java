/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ipo;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.JProgressBar;

/**
 *
 * @author s14003024
 */
class StatusListener implements PropertyChangeListener{
    
    JProgressBar bar;
    
    public StatusListener(JProgressBar bar){
        this.bar = bar;
    }

    @Override
    public void propertyChange(PropertyChangeEvent evt) {
        if("progress".equals(evt.getPropertyName())){
            bar.setValue((int)evt.getNewValue());
        }
    }


    
}
