/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ipo;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JProgressBar;
import javax.swing.SwingWorker;
import turnerColorSpace.*;

/**
 *
 * @author s14003024
 */
public class ImageWorker extends SwingWorker<String, String>{
    
    //initialize variables
    private ArrayList<File> imageFiles;
    private File outputFolder;
    private BufferedImage baseImage;
    private HashMap<String, Boolean> conversionMap;
    private double gammaValue;
    private ArrayList<ColorspaceTransform> transforms;
    private JProgressBar bar;
    private JButton runButton;
    private JButton cancelButton;
    
    public ImageWorker(ArrayList<File> imageFiles, File outputFolder, HashMap<String, Boolean> conversionTypes, double gammaValue, IPOView view){
        this.imageFiles = imageFiles;
        this.conversionMap = conversionTypes;
        this.outputFolder = outputFolder;
        this.gammaValue = gammaValue;
        transforms = new ArrayList();
        this.bar = view.getProgressBar();
        this.runButton = view.getRunButton();
        this.cancelButton = view.getCancelButton();
    }

    @Override
    protected String doInBackground() throws Exception {
        int index = 0;
        setProgress(0);
        for (File f : imageFiles){
            if (this.isCancelled()) break;
            
            transforms.clear();
            try {
//                    create the buffered image based on the selected file
                      baseImage = ImageIO.read(f);  
                      int height = baseImage.getHeight();
                      int width = baseImage.getWidth();
                      //create the Colorspace Transforms
                      if (conversionMap.get("HSI")){
                          transforms.add(new HSIToGray(width, height));
                      }
                      if (conversionMap.get("HSL")) {
                          transforms.add(new HSLToGray(width, height));
                      }
                      if (conversionMap.get("HSV")) {
                          transforms.add(new HSVToGray(width, height));
                      }
                      if (conversionMap.get("LAlphaBeta")) {
                          transforms.add(new LAlphaBetaToGray(width, height));
                      }
                      if (conversionMap.get("LMS")) {
                          transforms.add(new LMSToGray(width, height));
                      }
                      if (conversionMap.get("Lab")) {
                          transforms.add(new LabToGray(width, height));
                      }
                      if (conversionMap.get("Luv")) {
                          transforms.add(new LuvToGray(width, height));
                      }
                      if (conversionMap.get("RGB")) {
                          transforms.add(new RGBToGray(width, height));
                      }
                      if (conversionMap.get("XYZ")) {
                          transforms.add(new XYZToGray(width, height));
                      }
                      if (conversionMap.get("Ohta")) {
                          transforms.add(new OhtaToGray(width, height));
                      }
                      if (conversionMap.get("601")) {
                          transforms.add(new SixZeroOneToGray(width, height));
                      }
                      if (conversionMap.get("709")) {
                          transforms.add(new SevenZeroNineToGray(width, height));
                      }
                      if (conversionMap.get("rgbChroma")) {
                          transforms.add(new RGBChromaToGray(width, height));
                      }
                      
                      //Run through the image pixels and update pixels of necessary transforms
                      for (int h=0; h<height; h++){
                        for (int w=0; w<width; w++){
                            Color rgbPixel = new Color(baseImage.getRGB(w, h));
                            Color gammaRemovedPixel = TurnerUtil.invertGamma(rgbPixel);
                            
                            for (ColorspaceTransform ct : transforms){
                                ColorSpace cs = ct.getColorSpace();
                                if ((cs == ColorSpace.XYZ) || (cs == ColorSpace.Lab) || (cs == ColorSpace.Luv) || (cs == ColorSpace.LMS) || (cs == ColorSpace.LAlphaBeta)){
                                    ct.setPixelColor(gammaRemovedPixel, w, h);
                                } else {
                                    ct.setPixelColor(rgbPixel, w, h);
                                }
                            }
                        }
                    }
                      //write Grayscale (if requested)
                    if (conversionMap.get("Grayscale")){
                        for (ColorspaceTransform ct : transforms){
                            ct.writeGrayscaleImages(outputFolder, f.getName());
                        }
                    }
                      //write Binary (if requested)
                    if (conversionMap.get("Binary")){
                        OtsuThreshold otsu = new OtsuThreshold();
                        for (ColorspaceTransform ct : transforms){
                            GrayToBinary temp = new GrayToBinary(ct.getGrayscaleImages(), otsu);
                            System.out.println("Gray to Binary finished");
                            LargestObjectBinary temp2 = new LargestObjectBinary(temp);
                            
                            //temp.writeBinaryImages(outputFolder, f.getName() + ct.getColorSpace().toString() + "Binary");
                            temp2.writeBinaryImages(outputFolder, f.getName() + ct.getColorSpace().toString() + "Binary");
                        }
                    }  

                } catch (IOException ex) {
                    Logger.getLogger(IPOView.class.getName()).log(Level.SEVERE, null, ex);
                    JOptionPane.showMessageDialog(null, "Could not load " + f.getName());
                }
            
            ++index;
            publish("working");
            setProgress((int)(((index * 1.0)/imageFiles.size())*100));
            }
        
        System.out.println("done");
        
        return "done";
    }
    
    @Override
    public void done(){
        runButton.setEnabled(true);
        cancelButton.setEnabled(false);
    }
    
}

