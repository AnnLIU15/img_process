/*
    Copyright (C) 2015, Liang Fan
    All rights reverved.
*/

/**	\file		CTestFilter.cpp
    \brief		Implementes CTestFilter class.
*/

#include <streams.h>
#include <initguid.h>
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <dvdmedia.h>

#include "CTestFilter.h"
#include "MediaConvert.h"


const AMOVIESETUP_MEDIATYPE sudPinTypesIn[] =
{
    { &MEDIATYPE_Video, &MEDIASUBTYPE_IYUV   },
};

const AMOVIESETUP_MEDIATYPE sudPinTypesOut[] =
{
    {	&MEDIATYPE_Video, &MEDIASUBTYPE_RGB32  },
};

const AMOVIESETUP_PIN psudPins[] =
{
    { L"Input",  FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, NULL, 1,  sudPinTypesIn  },
    { L"Output", FALSE, TRUE,  FALSE, FALSE, &CLSID_NULL, NULL, 1,  sudPinTypesOut },
};

const AMOVIESETUP_FILTER sudFilter =
{
    &CLSID_TestFilter, // Filter CLSID
    L"Test Filter",    // Filter name
    MERIT_NORMAL,      // Merit
    2,                 // Number of pins
    psudPins           // Pin information
};

//
static WCHAR g_wszName[] = L"Test Filter";

CFactoryTemplate g_Templates[] =
{
    { g_wszName, &CLSID_TestFilter, CTestFilter::CreateInstance, NULL, &sudFilter },
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

//
static int filterInstances = 0;
////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

//
// DllRegisterServer
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);
}

//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

//
// Implements the CTestFilter class
//


// constructor
CTestFilter::CTestFilter(LPUNKNOWN pUnk, HRESULT *phr) :
    CTransformFilter(L"Test Filter", pUnk, CLSID_TestFilter)
{
    m_iWidth = 0;
    m_iHeight = 0;
    m_rtAvgTimePerFrame = 0;
    m_rtStart = 0;
    m_rtEnd = 0;
    m_pbIn = NULL;
    m_pbOut = NULL;
    m_iInputDataSize = 0;
    m_iOutputDataSize = 0;

    *phr = NOERROR;
}

// destructor
CTestFilter::~CTestFilter()
{
    if (filterInstances > 0)
        filterInstances--;

    // free buffers
    if (m_pbIn != NULL)
    {
        free(m_pbIn);
        m_pbIn = NULL;
    }
    if (m_pbOut != NULL)
    {
        free(m_pbOut);
        m_pbOut = NULL;
    }
}

//
// CreateInstance
//
// Never use more than two instances of this decoder in the same application
// Beccause of global variable problems!
CUnknown* WINAPI CTestFilter::CreateInstance(LPUNKNOWN punk, HRESULT* phr)
{
    if (filterInstances == 1)
    {
        *phr = E_FAIL;
        return NULL;
    }

    CTestFilter* pNewObject = new CTestFilter(punk, phr);
    if (pNewObject == NULL)
    {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

HRESULT  CTestFilter::CheckInputType(const CMediaType* mtIn)
{
    if (mtIn->majortype != MEDIATYPE_Video)
        return E_FAIL;
    if (mtIn->subtype != MEDIASUBTYPE_IYUV)
        return E_FAIL;

    // check width and height of video frames
    if (mtIn->formattype == FORMAT_VideoInfo &&
        mtIn->cbFormat >= sizeof(VIDEOINFOHEADER) &&
        mtIn->pbFormat != NULL)
    {
        VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mtIn->pbFormat;
        if (vih->bmiHeader.biWidth > MAX_WIDTH || vih->bmiHeader.biHeight > MAX_HEIGHT)
            return E_FAIL;
    }
    else if (mtIn->formattype == FORMAT_VideoInfo2 &&
        mtIn->cbFormat >= sizeof(VIDEOINFOHEADER2) &&
        mtIn->pbFormat != NULL)
    {
        VIDEOINFOHEADER2* vih2 = (VIDEOINFOHEADER2*)mtIn->pbFormat;
        if (vih2->bmiHeader.biWidth > MAX_WIDTH || vih2->bmiHeader.biHeight > MAX_HEIGHT)
            return E_FAIL;
    }
    else
    {
        return E_FAIL;
    }

    return NOERROR;
}

HRESULT  CTestFilter::CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut)
{
    HRESULT hr = CheckInputType(mtIn);
    if (FAILED(hr))
        return hr;

    //
    if (mtOut->majortype != MEDIATYPE_Video)
        return E_FAIL;
    if (mtOut->subtype != MEDIASUBTYPE_RGB32)
        return E_FAIL;

    return NOERROR;
}

HRESULT CTestFilter::GetMediaType(int iPosition, CMediaType* pmt)
{
    CAutoLock lock(&m_csFilter);

    if (iPosition < 0)
    {
        return E_INVALIDARG;
    }

    if (iPosition > 0)
    {
        return VFW_S_NO_MORE_ITEMS;
    }

    // set Media Type
    pmt->majortype = MEDIATYPE_Video;
    pmt->subtype = MEDIASUBTYPE_RGB32;
    pmt->bFixedSizeSamples = TRUE;
    pmt->bTemporalCompression = FALSE;
    pmt->formattype = FORMAT_VideoInfo;
    pmt->lSampleSize = m_iWidth * m_iHeight * 4;

    // append the extradata
    VIDEOINFOHEADER* mi = (VIDEOINFOHEADER*)pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
    if (mi == NULL)
        return E_FAIL;
    memset(mi, 0, sizeof(VIDEOINFOHEADER));
    mi->rcSource.left = 0;
    mi->rcSource.top = 0;
    mi->rcSource.right = m_iWidth;
    mi->rcSource.bottom = m_iHeight;
    mi->rcTarget = mi->rcSource;
    mi->bmiHeader.biBitCount = 32;
    mi->bmiHeader.biCompression = BI_RGB;
    mi->bmiHeader.biSizeImage = m_iWidth * m_iHeight * 4;
    mi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    mi->bmiHeader.biWidth = m_iWidth;
    mi->bmiHeader.biHeight = m_iHeight;
    mi->bmiHeader.biPlanes = 1;
    mi->AvgTimePerFrame = m_rtAvgTimePerFrame;

    return S_OK;
}

HRESULT CTestFilter::DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest)
{
    // Is the input pin connected
    if (!m_pInput->IsConnected())
    {
        return E_UNEXPECTED;
    }

    ASSERT(pAlloc);
    ASSERT(ppropInputRequest);
    HRESULT hr = NOERROR;

    ppropInputRequest->cbBuffer = m_iOutputDataSize;
    ppropInputRequest->cBuffers = 1;
    ppropInputRequest->cbAlign = 1;

    ASSERT(ppropInputRequest->cbBuffer);

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(ppropInputRequest, &Actual);
    if (FAILED(hr))
    {
        return hr;
    }

    ASSERT(Actual.cBuffers == 1);

    if (ppropInputRequest->cBuffers > Actual.cBuffers ||
        ppropInputRequest->cbBuffer > Actual.cbBuffer)
    {
        return E_FAIL;
    }

    return NOERROR;
}

HRESULT CTestFilter::CompleteConnect(PIN_DIRECTION direction, IPin* pReceivePin)
{
    if (direction == PINDIR_INPUT)
    {
        CMediaType mt = m_pInput->CurrentMediaType();
        if (mt.formattype == FORMAT_VideoInfo)
        {
            VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.pbFormat;
            m_rtAvgTimePerFrame = vih->AvgTimePerFrame;
            m_iWidth = vih->bmiHeader.biWidth;
            m_iHeight = abs(vih->bmiHeader.biHeight);
            m_iInputDataSize = m_iWidth * m_iHeight * 3 / 2;
            return S_OK;
        }
        else if (mt.formattype == FORMAT_VideoInfo2)
        {
            VIDEOINFOHEADER2* vih2 = (VIDEOINFOHEADER2*)mt.pbFormat;
            m_rtAvgTimePerFrame = vih2->AvgTimePerFrame;
            m_iWidth = vih2->bmiHeader.biWidth;
            m_iHeight = abs(vih2->bmiHeader.biHeight);
            m_iInputDataSize = m_iWidth * m_iHeight * 3 / 2;
            return S_OK;
        }
    }
    else
    {
        CMediaType mt = m_pOutput->CurrentMediaType();
        m_iOutputDataSize = m_iWidth * m_iHeight * 4;
        return S_OK;
    }

    return E_FAIL;
}

HRESULT CTestFilter::StartStreaming()
{
    // Set picture id and time stamp
    m_rtStart    = 0;
    m_rtEnd      = 0;
    
    fp = fopen("./outputs/results.csv","w");
    //fprintf(fp, "frame num,avg search point nums(frame),MAD(frame)\n");
    
    frame_num = 0;
    total_points = 0;
    total_SAD = 0;
    total_blocks = 0;
    cur_frame = (uint8_t*)malloc(sizeof(uint8_t) * m_iWidth * m_iHeight);
    past_frame = (uint8_t*)malloc(sizeof(uint8_t) * m_iWidth * m_iHeight);
    gray_prob_equal = (double_t*)malloc(sizeof(double_t) * m_iWidth * m_iHeight);
    memset(cur_frame, 0, sizeof(uint8_t) * m_iWidth * m_iHeight);
    memset(past_frame, 0, sizeof(uint8_t) * m_iWidth * m_iHeight);
    memset(gray_prob_equal, 0, sizeof(double_t) * 256);
    m_pbOut = (BYTE *)malloc(m_iOutputDataSize);
    if (m_pbOut == NULL)
        return E_FAIL;
    
    return S_OK;
}


HRESULT CTestFilter::StopStreaming()
{
    if (m_pbOut != NULL)
    {
        free(m_pbOut);
        m_pbOut = NULL;
    }
    /*fprintf(fp, "\navg search point nums(video),MAD(avg)\n");
    fprintf(fp, "%.4f,%.4f", float(total_points) / total_blocks, float(total_SAD) / (frame_num * m_iHeight * m_iWidth));
    */fclose(fp);
    
    return S_OK;
}

HRESULT CTestFilter::Receive(IMediaSample *pSample)
{
    HRESULT       hr;
    IMediaSample *pOut;
    BYTE         *pOutputBuffer;
    
    // Check for other streams and pass them on
    AM_SAMPLE2_PROPERTIES* const pProps = m_pInput->SampleProps();
    if (pProps->dwStreamId != AM_STREAM_MEDIA) 
    {
        return m_pOutput->Deliver(pSample);
    }
    
    // Receive the YUV data from source
    ASSERT(pSample);
    long lSourceSize = pSample->GetActualDataLength();
    BYTE *pSourceBuffer;
    pSample->GetPointer(&pSourceBuffer);
    // histogram
    double_t* tmp = ict_dsp::histogram(pSourceBuffer, m_iWidth, m_iHeight);
    double_t rho = 1;
    if (frame_num > 0)
    {
        rho = ict_dsp::cal_relative_rho(tmp, gray_prob_equal, 256);
        fprintf(fp, "%.6f\n", rho);
    }
    memcpy(gray_prob_equal,tmp, sizeof(double_t) * 256);
    
    int32_t* data;
    delete(tmp);
    if (frame_num == 0)    // first or change scene
    {
        // just store first frame(scene) copy and do nothing else 
        data = moEst::enhanced_HEXBS(pSourceBuffer, cur_frame, past_frame, m_iWidth, m_iHeight, 0, fp);
        // ICT 8X8
        ict_dsp::transformIct_8(pSourceBuffer, m_iWidth, m_iHeight);
        // Sleep(5000);
    }
    else                               // inter-frame code
    {
        // enhanced_HEXBS
        data = moEst::enhanced_HEXBS(pSourceBuffer, cur_frame, past_frame, m_iWidth, m_iHeight, frame_num, fp);
        //total_blocks += *data;      // blocks_frame;
        //total_SAD += *(data + 1);   //SAD_frame;
        //total_points += *(data + 2);//points_frame;
        
    }
    /*if(frame_num == 10)
        Sleep(50000);*/
    if (rho > 0.8)
    {
        frame_num += 1;
    }
    else
    {
        frame_num = 0;
    }
    free(data);
    // IYUV --> RGB32
    CAutoLock	lck(&m_csFilter);
    yuv420_to_rgb32(m_pbOut, pSourceBuffer, m_iWidth, m_iHeight);
    
    
    // Delivery the RGB data to downstream filter
    hr = m_pOutput->GetDeliveryBuffer(&pOut, NULL, NULL, 0);
    if (FAILED(hr))
    {
       return hr;
    }
    pOut->GetPointer(&pOutputBuffer);
    pOut->SetActualDataLength(m_iOutputDataSize);
    memcpy(pOutputBuffer, m_pbOut, m_iOutputDataSize);
    m_rtStart += m_rtAvgTimePerFrame;
    m_rtEnd    = m_rtStart + m_rtAvgTimePerFrame;
    pOut->SetTime(&m_rtStart, &m_rtEnd);
    hr = m_pOutput->Deliver(pOut);
    pOut->Release();
    
    return S_OK;
}