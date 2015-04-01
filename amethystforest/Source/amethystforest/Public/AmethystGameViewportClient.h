// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AmethystTypes.h"
#include "AmethystGameViewportClient.generated.h"

class SAmethystConfirmationDialog;

struct FAmethystGameLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FAmethystGameLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		ResourceObject = LoadObject<UObject>(NULL, *InTextureName.ToString());
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		if (ResourceObject)
		{
			Collector.AddReferencedObject(ResourceObject);
		}
	}
};

class SAmethystLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAmethystLoadingScreen) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

private:
	EVisibility GetLoadIndicatorVisibility() const
	{
		return EVisibility::Visible;
	}

	/** loading screen image brush */
	TSharedPtr<FSlateDynamicImageBrush> LoadingScreenBrush;
};

UCLASS(Within = Engine, transient, config = Engine)
class UAmethystGameViewportClient : public UGameViewportClient
{
	GENERATED_UCLASS_BODY()

public:

	// start UGameViewportClient interface
	void NotifyPlayerAdded(int32 PlayerIndex, ULocalPlayer* AddedPlayer) override;
	void AddViewportWidgetContent(TSharedRef<class SWidget> ViewportContent, const int32 ZOrder = 0) override;
	void RemoveViewportWidgetContent(TSharedRef<class SWidget> ViewportContent) override;

	void ShowDialog(TWeakObjectPtr<ULocalPlayer> PlayerOwner, EAmethystDialogType::Type DialogType, const FText& Message, const FText& Confirm, const FText& Cancel, const FOnClicked& OnConfirm, const FOnClicked& OnCancel);
	void HideDialog();

	void ShowLoadingScreen();
	void HideLoadingScreen();

	bool IsShowingDialog() const { return DialogWidget.IsValid(); }

	EAmethystDialogType::Type GetDialogType() const;
	TWeakObjectPtr<ULocalPlayer> GetDialogOwner() const;

	TSharedPtr<SAmethystConfirmationDialog> GetDialogWidget() { return DialogWidget; }

	//FTicker Funcs
	virtual void Tick(float DeltaSeconds) override;

#if WITH_EDITOR
	virtual void DrawTransition(class UCanvas* Canvas) override;
#endif //WITH_EDITOR
	// end UGameViewportClient interface

protected:
	void HideExistingWidgets();
	void ShowExistingWidgets();

	/** List of viewport content that the viewport is tracking */
	TArray<TSharedRef<class SWidget>>				ViewportContentStack;

	TArray<TSharedRef<class SWidget>>				HiddenViewportContentStack;

	TSharedPtr<class SWidget>						OldFocusWidget;

	/** Dialog widget to show temporary messages ("Controller disconnected", "Parental Controls don't allow you to play online", etc) */
	TSharedPtr<SAmethystConfirmationDialog>			DialogWidget;

	TSharedPtr<SAmethystLoadingScreen>				LoadingScreenWidget;
};
