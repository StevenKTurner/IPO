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
import java.util.HashMap;
import java.util.Map;
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
            HashMap<Integer, ArrayList<Integer>> labels = new HashMap();
            HashMap<Integer, Integer> sizes = new HashMap();
            
            System.out.println("Starting on channel " + i);
            
            for (int h = 0; h < binarySource[i].getHeight(); h++){
                for (int w = 0; w < binarySource[i].getWidth(); w++){
                    
                    //Four-way check
                    //initialize variables for check
                    int sample = sourceRaster.getSample(w, h, 0);
                    
                    //System.out.println(sample);
                    
                    //Check neighbors and label similar
                    if (sample < 1){ //if there's a black pixel
                        
                        //System.out.println("Black pixel found");
                        
                        //create other check variables here
                        int north;
                        int west;
                    
                        //make sure we're not on the edge, mark when pixel doesn't have a west or north neighbor with a 1
                        if (h == 0){
                            north = 1;
                        } else north = sourceRaster.getSample(w, h-1, 0);
                        if (w == 0){
                            west = 1;
                        } else west = sourceRaster.getSample(w-1, h, 0);
                        
                        //Check neighboring pixels...
                        if (west < 1){
                            labeledPixels[w][h] = labeledPixels[w-1][h]; //set label to same as west neighbor
                        }
                        if (north < 1){
                            labeledPixels[w][h] = labeledPixels[w][h-1]; //set label to same as north neighbor
                        }
                        if (north >= 1 && west >= 1) {
                            labeledPixels[w][h] = currentLabel; //no neighbors, set to a new label
                            currentLabel++; //change current label so next new label will be different
                        }
                        
                        
                        //set equivalence
                        if (west < 1 && north < 1){
                            //labels.put(labeledPixels[w-1][h], labeledPixels[w][h-1]);
                            //labels.put(labeledPixels[w-1][h], labeledPixels[w][h-1]); //Ah, this won't work!  It will replace equivalence with the latest value!
                            //labels.put(labeledPixels[w][h-1], labeledPixels[w-1][h]);
                            //System.out.println(labeledPixels[w-1][h] + " is equal to " + labeledPixels[w][h-1]);
                            //labels.put(labeledPixels[w][h-1], labeledPixels[w-1][h]); //don't think I need this
                            
                            //Check to see if key (the label value) is in labels hashtable
                            if (labels.containsKey(labeledPixels[w-1][h])){
                                //if so, add the other label value to the first value's arraylist
                                ArrayList<Integer> temp = labels.get(labeledPixels[w-1][h]);
                                temp.add(labeledPixels[w][h-1]);
                            } else { //if not, create a new arraylist under that key and add the value
                                labels.put(labeledPixels[w-1][h], new ArrayList<Integer>());
                                labels.get(labeledPixels[w-1][h]).add(labeledPixels[w][h-1]);
                            }
                            //
                        }
                    }
                    
                }
            }
            
            System.out.println("First pass finished");
            
            //Second Pass - unify touching object labels and find biggest object
            for (int h = 0; h < binarySource[i].getHeight(); h++){
                for (int w = 0; w < binarySource[i].getWidth(); w++){
                    
                    //for each labelled pixel
                    if (labeledPixels[w][h] > 0){
                        
//                        //Check if it's in the equivalence hashtable, and if so set it to the value decided in that table
//                        if (labels.containsKey(labeledPixels[w][h])){
//                            //labeledPixels[w][h] = labels.get(labeledPixels[w][h]);
//                            
//                            
//                        }
                        
                        //search through the labels to see if the label value is in an arraylist, and if so set it to the HashMap key
                        for (Map.Entry<Integer, ArrayList<Integer>> entry : labels.entrySet()){
                            if (entry.getValue().contains(labeledPixels[w][h])){
                                labeledPixels[w][h] = entry.getKey();
                                break; //break to set it to first found value
                            }
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
            
            System.out.println("Second Pass Finished");
            
            //Find the biggest label --This isn't working sometimes, figure out why.
            int biggest = 1;
            Set<Integer> keys = sizes.keySet();
            
            if (!(sizes.containsKey(biggest))){
                biggest = (int) keys.toArray()[0];
            }
            
            for (int reference : keys){
                //System.out.println(reference);
                if (sizes.get(reference) > sizes.get(biggest)) {
                    biggest = reference;
                    //System.out.println("Biggest is now " + biggest);
                }
            }
            
            //test section
            for (int reference : keys){
                System.out.println(reference + " has " + sizes.get(reference) + " points");
            }
            
//            System.out.println("Biggest Label found");
//            System.out.println(biggest);
            
            //Third pass - write only the largest object to the image raster for Binary Output
            for (int h = 0; h < binarySource[i].getHeight(); h++){
                for (int w = 0; w < binarySource[i].getWidth(); w++){
                    //System.out.println("Label: " + labeledPixels[w][h]);
                    if (labeledPixels[w][h] == biggest){
                        outputRaster.setSample(w, h, 0, 0);
                    } else {
                        outputRaster.setSample(w, h, 0, 1);
                    }
                }
            }
            
            System.out.println("Third Pass finished");
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
