/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author s14003024
 */
public class LAlphaBetaToGray implements ColorspaceToGray{
    
    private BufferedImage LChannel;
    private BufferedImage AlphaChannel;
    private BufferedImage BetaChannel;
    private WritableRaster Lr;
    private WritableRaster Alphar;
    private WritableRaster Betar;
    
    private static final double multiOne = 1.0/Math.sqrt(3);
    private static final double multiTwo = 1.0/Math.sqrt(6);
    private static final double multiThree = 1.0/Math.sqrt(2);
    
    private BufferedImage[] LAlphaBetaChannels = new BufferedImage[3];
    
    //test variables
    static double lt;
    static double alphat;
    static double betat;
    
    public LAlphaBetaToGray(int width, int height){
        LChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        AlphaChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        BetaChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        LAlphaBetaChannels[0] = LChannel;
        LAlphaBetaChannels[1] = AlphaChannel;
        LAlphaBetaChannels[2] = BetaChannel;
        
        Lr = LChannel.getRaster();
        Alphar = AlphaChannel.getRaster();
        Betar = BetaChannel.getRaster();
    }
    
    public void setPixelColor(LMSColor lms, int x, int y){
        
        double l = (multiOne * Math.log(lms.getL())) + (multiOne * Math.log(lms.getM())) + (multiOne * Math.log(lms.getS()));
        double alpha = (multiTwo * Math.log(lms.getL())) + (multiTwo * Math.log(lms.getM())) - (2 * multiTwo * Math.log(lms.getS()));
        double beta = (multiThree * Math.log(lms.getL())) - (multiThree * Math.log(lms.getM()));
        
        lt = l;
        alphat = alpha;
        betat = beta;
        
        //This is wrong, I need to normalize the channels (once I figure out the lower limit of L)
        Lr.setSample(x, y, 0, (int) (l*255));
        Alphar.setSample(x, y, 0, (int) (alpha*255));
        Betar.setSample(x, y, 0, (int) (beta*255));
    }

    public void setPixelColor(XYZColor xyz, int x, int y){
        LMSColor lms = new LMSColor(xyz);
        setPixelColor(lms, x, y);
    }
    
    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        setPixelColor(new XYZColor(R, G, B), x, y);
    }

    @Override
    public void setPixelColor(Color c, int x, int y) {
        setPixelColor(new XYZColor(c), x, y);
    }

    @Override
    public void setPixelColor(int rgb, int x, int y) {
        Color c = new Color(rgb);
        setPixelColor(c, x, y);
    }

    @Override
    public BufferedImage[] getGrayscaleImages() {
        return LAlphaBetaChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(LChannel, "PNG", new File(prefix + "LAlphaBetaL.png"));
            ImageIO.write(AlphaChannel, "PNG", new File(prefix + "LAlphaBetaAlpha.png"));
            ImageIO.write(BetaChannel, "PNG", new File(prefix + "LAlphaBetaBeta.png"));
        } catch (IOException ex) {
            Logger.getLogger(LAlphaBetaToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("LAlphaBeta could not write images");
        }
    }
    
    public static void main(String[] args) {
        LAlphaBetaToGray test = new LAlphaBetaToGray(1,1);
        double lmax = 0.0;
        double lmin = 0.0;
        double alphaMax = 0.0;
        double alphaMin = 0.0;
        double betaMax = 0.0;
        double betaMin = 0.0;
        for(int ri = 0; ri < 256; ri++){
            for (int gi = 0; gi < 256; gi++){
                for (int bi = 0; bi < 256; bi++){
                    test.setPixelColor(new Color(ri, gi, bi), 0, 0);
                    if (lmax < lt) lmax = lt;
                    if (lmin > lt) lmin = lt;
                    if (alphaMax < alphat) alphaMax = alphat;
                    if (alphaMin > alphat) alphaMin = alphat;
                    if (betaMax < betat) betaMax = betat;
                    if (betaMin > betat) betaMin = betat;
                }
            }
        }
        System.out.println("Lmax = " + lmax);
        System.out.println("Lmin = " + lmin);
        System.out.println("Alphamax = " + alphaMax);
        System.out.println("Alphamin = " + alphaMin);
        System.out.println("betamax = " + betaMax);
        System.out.println("betamin = " + betaMin);
    }
    
}
