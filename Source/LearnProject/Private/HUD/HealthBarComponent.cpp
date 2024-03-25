// Fill out your copyright notice in the Description page of Project Settings.


#include "LearnProject/Public/HUD/HealthBarComponent.h"
#include "Components/ProgressBar.h"
#include "LearnProject/Public/HUD/HealthBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
    if(HealthBarWidget == nullptr)
    {
    // HealthBarWidget->SetPercent(Percent);
    HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
    }
    if(HealthBarWidget && HealthBarWidget->HealthBar)
    {
    HealthBarWidget->HealthBar->SetPercent(Percent);
    }
}
