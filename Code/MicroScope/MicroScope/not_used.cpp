
=====================暂时不使用的代码=======================
void CMicroScopeDlg::AutoGainOnce(Camera_t* camera)
{
    // Check whether the gain auto function is available.
    if ( !IsWritable( camera->GainAuto))
    {
        cout << "The camera does not support Gain Auto." << endl << endl;
        return;
    }

    // Maximize the grabbed image area of interest (Image AOI).
    if (IsWritable(camera->OffsetX))
    {
        camera->OffsetX.SetValue(camera->OffsetX.GetMin());
    }
    if (IsWritable(camera->OffsetY))
    {
        camera->OffsetY.SetValue(camera->OffsetY.GetMin());
    }
    camera->Width.SetValue(camera->Width.GetMax());
    camera->Height.SetValue(camera->Height.GetMax());

    if(IsAvailable(camera->AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for luminance statistics.
        // We want to use ROI1 for gathering the statistics
        if (IsWritable(camera->AutoFunctionROIUseBrightness))
        {
            camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
            camera->AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
            camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
            camera->AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
        }

        // Set the ROI (in this example the complete sensor is used)
        camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
        camera->AutoFunctionROIOffsetX.SetValue(0);
        camera->AutoFunctionROIOffsetY.SetValue(0);
        camera->AutoFunctionROIWidth.SetValue(camera->Width.GetMax());
        camera->AutoFunctionROIHeight.SetValue(camera->Height.GetMax());
    }


    // Set the target value for luminance control.
    // A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
    // A value of 0.4 means 40 % and so forth.
    camera->AutoTargetBrightness.SetValue(m_nAutoBrightness/100.0);	//0.3

    // We are going to try GainAuto = Once.

    cout << "Trying 'GainAuto = Once'." << endl;
    cout << "Initial Gain = " << camera->Gain.GetValue() << endl;

    // Set the gain ranges for luminance control.
    camera->AutoGainLowerLimit.SetValue(camera->Gain.GetMin());
    camera->AutoGainUpperLimit.SetValue(camera->Gain.GetMax());

    camera->GainAuto.SetValue(GainAuto_Once);

    // When the "once" mode of operation is selected,
    // the parameter values are automatically adjusted until the related image property
    // reaches the target value. After the automatic parameter value adjustment is complete, the auto
    // function will automatically be set to "off" and the new parameter value will be applied to the
    // subsequently grabbed images.

    int n = 0;
    while (camera->GainAuto.GetValue() != GainAuto_Off)
    {
        GrabResultPtr_t ptrGrabResult;
        camera->GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        //Pylon::DisplayImage(1, ptrGrabResult);
#endif
        ++n;
        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);

        //Make sure the loop is exited.
        if (n > 100)
        {
            throw RUNTIME_EXCEPTION( "The adjustment of auto gain did not finish.");
        }
    }

    cout << "GainAuto went back to 'Off' after " << n << " frames." << endl;
    cout << "Final Gain = " << camera->Gain.GetValue() << endl << endl;
}
void CMicroScopeDlg::AutoExposureOnce(Camera_t* camera)
{
    // Check whether auto exposure is available
    if ( !IsWritable( camera->ExposureAuto))
    {
        cout << "The camera does not support Exposure Auto." << endl << endl;
        return;
    }

    // Maximize the grabbed area of interest (Image AOI).
    if (IsWritable(camera->OffsetX))
    {
        camera->OffsetX.SetValue(camera->OffsetX.GetMin());
    }
    if (IsWritable(camera->OffsetY))
    {
        camera->OffsetY.SetValue(camera->OffsetY.GetMin());
    }
    camera->Width.SetValue(camera->Width.GetMax());
    camera->Height.SetValue(camera->Height.GetMax());

    if(IsAvailable(camera->AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for luminance statistics.
        // We want to use ROI1 for gathering the statistics.
        if (IsWritable(camera->AutoFunctionROIUseBrightness))
        {
            camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
            camera->AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
            camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
            camera->AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
        }

        // Set the ROI (in this example the complete sensor is used)
        camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
        camera->AutoFunctionROIOffsetX.SetValue(0);
        camera->AutoFunctionROIOffsetY.SetValue(0);
        camera->AutoFunctionROIWidth.SetValue(camera->Width.GetMax());
        camera->AutoFunctionROIHeight.SetValue(camera->Height.GetMax());
    }

    // Set the target value for luminance control.
    // A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
    // A value of 0.4 means 40 % and so forth.
    camera->AutoTargetBrightness.SetValue(m_nAutoBrightness/100.0);		//0.3

    // Try ExposureAuto = Once.
    cout << "Trying 'ExposureAuto = Once'." << endl;
    cout << "Initial exposure time = ";
    cout << camera->ExposureTime.GetValue() << " us" << endl;

    // Set the exposure time ranges for luminance control.
    camera->AutoExposureTimeLowerLimit.SetValue(camera->AutoExposureTimeLowerLimit.GetMin());
    camera->AutoExposureTimeUpperLimit.SetValue(camera->AutoExposureTimeLowerLimit.GetMax());

    camera->ExposureAuto.SetValue(ExposureAuto_Once);

    // When the "once" mode of operation is selected,
    // the parameter values are automatically adjusted until the related image property
    // reaches the target value. After the automatic parameter value adjustment is complete, the auto
    // function will automatically be set to "off", and the new parameter value will be applied to the
    // subsequently grabbed images.
    int n = 0;
    while (camera->ExposureAuto.GetValue() != ExposureAuto_Off)
    {
        GrabResultPtr_t ptrGrabResult;
        camera->GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        //Pylon::DisplayImage(1, ptrGrabResult);
#endif
        ++n;

        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);

        //Make sure the loop is exited.
        if (n > 100)
        {
            throw RUNTIME_EXCEPTION( "The adjustment of auto exposure did not finish.");
        }
    }
    cout << "ExposureAuto went back to 'Off' after " << n << " frames." << endl;
    cout << "Final exposure time = ";
    cout << camera->ExposureTime.GetValue() << " us" << endl << endl;
}

void CMicroScopeDlg::StopCam()
{
    if (m_camera.IsGrabbing())
    {
		m_camera.ExposureAuto.SetValue(ExposureAuto_Off);
        m_camera.StopGrabbing();
    }
	m_bitmapImage.Release();
	m_ptrGrabResult.Release();

	if (m_camera.IsOpen())
		m_camera.Close();
	m_camera.DestroyDevice();
}

void CMicroScopeDlg::AutoWhiteBalance(Camera_t* camera)
{
    // Check whether the Balance White Auto feature is available.
    if ( !IsWritable( camera->BalanceWhiteAuto))
    {
        cout << "The camera does not support Balance White Auto." << endl << endl;
        return;
    }

    // Maximize the grabbed area of interest (Image AOI).
    if (IsWritable(camera->OffsetX))
    {
        camera->OffsetX.SetValue(camera->OffsetX.GetMin());
    }
    if (IsWritable(camera->OffsetY))
    {
        camera->OffsetY.SetValue(camera->OffsetY.GetMin());
    }
    camera->Width.SetValue(camera->Width.GetMax());
    camera->Height.SetValue(camera->Height.GetMax());

    if(IsAvailable(camera->AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for white balance.
        // We want to use ROI2
        camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
        camera->AutoFunctionROIUseWhiteBalance.SetValue(false);   // ROI 1 is not used for white balance
        camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
        camera->AutoFunctionROIUseWhiteBalance.SetValue(true);   // ROI 2 is used for white balance

        // Set the Auto Function AOI for white balance statistics.
        // Currently, AutoFunctionROISelector_ROI2 is predefined to gather
        // white balance statistics.
        camera->AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
        camera->AutoFunctionROIOffsetX.SetValue(0);
        camera->AutoFunctionROIOffsetY.SetValue(0);
        camera->AutoFunctionROIWidth.SetValue(camera->Width.GetMax());
        camera->AutoFunctionROIHeight.SetValue(camera->Height.GetMax());
    }

    cout << "Trying 'BalanceWhiteAuto = Once'." << endl;
    cout << "Initial balance ratio: ";
    camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
        cout << "R = " << camera->BalanceRatio.GetValue() << "   ";
    camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
        cout << "G = " << camera->BalanceRatio.GetValue() << "   ";
    camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
        cout << "B = " << camera->BalanceRatio.GetValue() << endl;

    camera->BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Once);

    // When the "once" mode of operation is selected,
    // the parameter values are automatically adjusted until the related image property
    // reaches the target value. After the automatic parameter value adjustment is complete, the auto
    // function will automatically be set to "off" and the new parameter value will be applied to the
    // subsequently grabbed images.
    int n = 0;
    while (camera->BalanceWhiteAuto.GetValue() != BalanceWhiteAuto_Off)
    {
        GrabResultPtr_t ptrGrabResult;
        camera->GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        //Pylon::DisplayImage(1, ptrGrabResult);
#endif
        ++n;

        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);

        //Make sure the loop is exited.
        if (n > 100)
        {
            throw RUNTIME_EXCEPTION( "The adjustment of auto white balance did not finish.");
        }
    }
    cout << "BalanceWhiteAuto went back to 'Off' after ";
    cout << n << " frames." << endl;
    cout << "Final balance ratio: ";
    camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
    cout << "R = " << camera->BalanceRatio.GetValue() << "   ";
    camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
    cout << "G = " << camera->BalanceRatio.GetValue() << "   ";
    camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
    cout << "B = " << camera->BalanceRatio.GetValue() << endl;
}
