/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author s14003024
 */
public class LargestObjectBinary {
    
    BufferedImage[] binarySource;
    BufferedImage[] binaryImages;
    
    /**
     *
     * @param gtb
     */
    public LargestObjectBinary(GrayToBinary gtb){
        binarySource = gtb.getBinaryImages();
        binaryImages = new BufferedImage[binarySource.length];
        
        System.out.println("Largest binary initialized");
        
        for (int i = 0; i < binarySource.length; i++){
            binaryImages[i] = new BufferedImage(binarySource[i].getWidth(), binarySource[i].getHeight(), BufferedImage.TYPE_BYTE_BINARY);
            WritableRaster sourceRaster = binarySource[i].getRaster();
            WritableRaster outputRaster = binaryImages[i].getRaster();
            
            //using algorithm for connectivity as described on https://en.wikipedia.org/wiki/Connected-component_labeling
            int[][] labeledPixels = new int[binarySource[i].getWidth()][binarySource[i].getHeight()];
            int currentLabel = 1;
            Hashtable<Integer, Integer> labels = new Hashtable();
            Hashtable<Integer, Integer> sizes = new Hashtable();
            
            System.out.println("Starting on channel " + i);
            
            for (int h = 0; h < binarySource[i].getWidth(); h++){
                for (int w = 0; w < binarySource[i].getHeight(); w++){
                    
                    //Four-way check
                    //initialize variables for check
                    int sample = sourceRaster.getSample(w, h, 0);
                    
                    //Check neighbors and label similar
                    if (sample == 1){ //if there's a black pixel
                        
                        //System.out.println("Black pixel found");
                        
                        //create other check variables here
                        int north;
                        int west;
                    
                        //make sure we're not on the edge, mark when pixel doesn't have a west or north neighbor with a -1
                        if (h == 0){
                            north = -1;
                        } else north = sourceRaster.getSample(w, h-1, 0);
                        if (w == 0){
                            west = -1;
                        } else west = sourceRaster.getSample(w-1, h, 0);
                        
                        //Check neighboring pixels...
                        if (west >= 1){
                            labeledPixels[w][h] = labeledPixels[w-1][h]; //set label to same as west neighbor
                        } else if (north >= 1){
                            labeledPixels[w][h] = labeledPixels[w][h-1]; //set label to same as north neighbor
                        } else {
                            labeledPixels[w][h] = currentLabel; //no neighbors, set to a new label
                            currentLabel++; //change current label so next new label will be different
                        }
                        
                        //System.out.println(labeledPixels[w][h]);
                        
                        //set equivalence
                        if (west >= 1 && north >= 1){
                            labels.put(labeledPixels[w-1][h], labeledPixels[w][h-1]);
                            //labels.put(labeledPixels[w][h-1], labeledPixels[w-1][h]); //don't think I need this
                        }
                    }
                    
                }
            }
            
            System.out.println("First pass finished");
            
            //Second Pass - unify touching object labels and find biggest object
            for (int h = 0; h < labeledPixels.length; h++){
                for (int w = 0; w < labeledPixels[h].length; w++){
                    
                    //for each labelled pixel
                    if (labeledPixels[w][h] > 0){
                        
                        //Check if it's in the equivalence hashtable, and if so set it to the value decided in that table
                        if (labels.containsKey(labeledPixels[w][h])){
                            labeledPixels[w][h] = labels.get(labeledPixels[w][h]);
                        }
                        
                        //mark size increase for that label
                        if (sizes.containsKey(labeledPixels[w][h])){
                            int tempAdd = sizes.get(labeledPixels[w][h]) + 1;
                            sizes.put(labeledPixels[w][h], tempAdd);
                        } else {
                            sizes.put(labeledPixels[w][h], 1);
                        }
                    }
                
                }
            }
            //Find the biggest label
            int biggest = 0;
            Set<Integer> keys = sizes.keySet();
            for (int size : keys){
                if (sizes.get(size) > biggest) biggest = sizes.get(size);
            }
            
            //Third pass - write only the largest object to the image raster for Binary Output
            for (int h = 0; h < labeledPixels.length; h++){
                for (int w = 0; w < labeledPixels[h].length; w++){
                    if (labeledPixels[w][h] == biggest){
                        outputRaster.setSample(w, h, 0, 1);
                    } else {
                        outputRaster.setSample(w, h, 0, 0);
                    }
                }
            }
        }
    }
    
    public void writeBinaryImages(String prefix){
        try {
            for (int i = 0; i < binaryImages.length; i++){
                ImageIO.write(binaryImages[i], "PNG", new File(prefix + i + ".png"));
            }
        } catch (IOException ex) {
            Logger.getLogger(GrayToBinary.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Gray to Binary could not write images");
        }
    }
    
    public void writeBinaryImages(File location, String prefix){
        try {
            for (int i = 0; i < binaryImages.length; i++){
                ImageIO.write(binaryImages[i], "PNG", new File(location, prefix + i + ".png"));
            }
        } catch (IOException ex) {
            Logger.getLogger(GrayToBinary.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Gray to Binary could not write images");
        }
    }
}
