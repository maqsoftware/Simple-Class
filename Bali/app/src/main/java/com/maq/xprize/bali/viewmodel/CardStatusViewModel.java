/*
 * Copyright 2017, Team Chimple
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.maq.xprize.bali.viewmodel;

import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.MutableLiveData;
import android.arch.lifecycle.ViewModel;

public class CardStatusViewModel extends ViewModel {
    public static final int NONE = 0;
    public static final int CORRECT_CHOICE = 1;
    public static final int READY_TO_GO = 2;
    public static final int INCORRECT_CHOICE = 3;

    private final MutableLiveData<Integer> mViewed = new MutableLiveData<Integer>();

    public void viewed(Integer v) {
        mViewed.setValue(v);
    }

    public LiveData<Integer> getViewed() {
        return mViewed;
    }
}
