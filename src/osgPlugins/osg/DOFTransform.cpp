#include "osg/DOFTransform"

#include "osgDB/Registry"
#include "osgDB/Input"
#include "osgDB/Output"

using namespace osg;
using namespace osgDB;
using namespace std;

// forward declare functions to use later.
bool DOFTransform_readLocalData(Object& obj, Input& fr);
bool DOFTransform_writeLocalData(const Object& obj, Output& fw);

// register the read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy g_DOFTransformProxy
(
    new osg::DOFTransform,
    "DOFTransform",
    "Object Node Transform DOFTransform Group",
    &DOFTransform_readLocalData,
    &DOFTransform_writeLocalData,
    DotOsgWrapper::READ_AND_WRITE
);

bool DOFTransform_readLocalData(Object& obj, Input& fr)
{
    bool iteratorAdvanced = false;

    DOFTransform& dof = static_cast<DOFTransform&>(obj);

    if (fr.matchSequence("PutMatrix {"))
    {
        fr += 2; // skip over "putMatrix {"
        iteratorAdvanced = true;
    
        bool matched = true;
        for(int k=0;k<16 && matched;++k)
        {
            matched = fr[k].isFloat();
        }
        if (matched)
        {
            osg::Matrix matrix;
            int k=0;
            for(int i=0;i<4;++i)
            {
                for(int j=0;j<4;++j)
                {
                    fr[k].getFloat(matrix(i,j));
                    k++;
                }
            }
            
            dof.setPutMatrix(matrix);
            dof.setInversePutMatrix(Matrix::inverse(matrix));
        }
        
        fr.advanceToEndOfCurrentBlock();
    }


#define ReadVec3(A,B) {  \
    if (fr[0].matchWord(B) && \
        fr[1].getFloat(vec3[0]) && \
        fr[2].getFloat(vec3[1]) && \
        fr[3].getFloat(vec3[2])) \
    { \
        dof.A(vec3); \
        fr+=4; \
        iteratorAdvanced = true; \
    } \
} 

    Vec3 vec3;
    
    ReadVec3(setMinHPR,"minHPR")
    ReadVec3(setMaxHPR,"maxHPR")
    ReadVec3(setIncrementHPR,"incrementHPR")
    ReadVec3(setCurrentHPR,"currentHPR")
    
    ReadVec3(setMinTranslate,"minTranslate")
    ReadVec3(setMaxTranslate,"maxTranslate")
    ReadVec3(setIncrementTranslate,"incrementTranslate")
    ReadVec3(setCurrentTranslate,"currentTranslate")
    
    ReadVec3(setMinScale,"minScale")
    ReadVec3(setMaxScale,"maxScale")
    ReadVec3(setIncrementScale,"incrementScale")
    ReadVec3(setCurrentScale,"currentScale")
    

    if (fr.matchSequence("limitationFlags %i"))
    {
        unsigned int flags;
        fr[1].getUInt(flags);
        dof.setLimitationFlags(flags);
    
        fr += 2;
        iteratorAdvanced = true;
        
    }
    
    if (fr[0].matchWord("animationOn"))
    {
    
        if (fr[1].matchWord("TRUE")) dof.setAnimationOn(true);
        else if (fr[1].matchWord("FALSE")) dof.setAnimationOn(false);
    
        fr += 2;
        iteratorAdvanced = true;
        
    }
    
#undef ReadVec3

    return iteratorAdvanced;
}


bool DOFTransform_writeLocalData(const Object& obj, Output& fw)
{
    const DOFTransform& transform = static_cast<const DOFTransform&>(obj);

    fw.indent()<<"PutMatrix "<<transform.getPutMatrix()<<std::endl;

    fw.indent()<<"minHPR             "<<transform.getMinHPR()<<std::endl;
    fw.indent()<<"maxHPR             "<<transform.getMaxHPR()<<std::endl;
    fw.indent()<<"incrementHPR       "<<transform.getIncrementHPR()<<std::endl;
    fw.indent()<<"currentHPR         "<<transform.getCurrentHPR()<<std::endl;

    fw.indent()<<"minTranslate       "<<transform.getMinTranslate()<<std::endl;
    fw.indent()<<"maxTranslate       "<<transform.getMaxTranslate()<<std::endl;
    fw.indent()<<"incrementTranslate "<<transform.getIncrementTranslate()<<std::endl;
    fw.indent()<<"currentTranslate   "<<transform.getCurrentTranslate()<<std::endl;

    fw.indent()<<"minScale           "<<transform.getMinScale()<<std::endl;
    fw.indent()<<"axScale            "<<transform.getMaxScale()<<std::endl;
    fw.indent()<<"incrementScale     "<<transform.getIncrementScale()<<std::endl;
    fw.indent()<<"currentScale       "<<transform.getCurrentScale()<<std::endl;

    fw.indent()<<"limitationFlags    0x"<<hex<<transform.getLimitationFlags()<<dec<<std::endl;

    fw.indent()<<"animationOn        ";
    if (transform.getAnimationOn()) fw<<"TRUE"<<std::endl;
    else fw<<"FALSE"<<std::endl;

    return true;
}
